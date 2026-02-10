#include <functional>
#include <unordered_set>

#include "ReconstructParentChildNameStrategy.h"

#include "Utils/Utils.h"

#include "RLog/RLog.h"
#include "extern.h"

#include "../NodeProcessor.h"

bool ReconstructParentChildNameStrategy::collectNodeNames(MR::NetworkDef* netDef, NodeProcessor* processor)
{
	std::map<MR::NodeID, std::string>& nodeNameMap = processor->getNodeNameMap();
	nodeNameMap.clear();

	MR::NodeDef* rootNodeDef = netDef->getNodeDef(netDef->getRootNodeID());

	std::function<void(MR::NodeDef*)> collectNames;
	collectNames = [&](MR::NodeDef* nodeDef)
		{
			MR::NodeID id = nodeDef->getNodeID();

			if (processor->getNodeName(id) != "")
				return;

			if (nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
				return;

			std::string nodeName = netDef->getNodeNameFromNodeID(nodeDef->getNodeID());

			if (processor->isNodeBlendTree(nodeDef))
			{
				// Normal names are stripped in the DS2 nmb, so we just have to get the standard name.
				std::string blendTreeName = NodeNameStrategyUtils::getNodeNameFromFullPath(nodeName);

				if (processor->isNodeStateNode(nodeDef))
					blendTreeName = NodeNameStrategyUtils::getStateNodeNameFromStringTable(netDef, nodeDef->getNodeID());

				processor->registerBlendTreeName(nodeDef->getNodeID(), NodeNameStrategyUtils::getNodeNameFromFullPath(nodeName));
			}

			processor->registerNodeName(nodeDef->getNodeID(), NodeNameStrategyUtils::getNodeNameFromFullPath(nodeName));

			if (nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
				processor->registerStateMachineName(nodeDef->getNodeID(), NodeNameStrategyUtils::getNodeNameFromFullPath(nodeName));

			if (!nodeName.empty())
			{
				MR::NodeDef* parentNodeContainer = processor->getParentNodeContainer(nodeDef);

				while (parentNodeContainer && parentNodeContainer->getNodeID() != netDef->getRootNodeID())
				{
					const std::string parentPath = NodeNameStrategyUtils::getParentNodeNameFromFullPath(nodeName);
					std::string nameWithoutParent = NodeNameStrategyUtils::getNodeNameFromFullPath(parentPath);

					if (processor->isNodeBlendTree(parentNodeContainer))
					{
						processor->registerBlendTreeName(parentNodeContainer->getNodeID(), nameWithoutParent);

						if (parentNodeContainer->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
							processor->registerStateMachineName(parentNodeContainer->getNodeID(), NodeNameStrategyUtils::getNodeNameFromFullPath(nameWithoutParent));

						nameWithoutParent = "";
					}

					processor->registerNodeName(parentNodeContainer->getNodeID(), nameWithoutParent);

					if (parentNodeContainer->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
						processor->registerStateMachineName(parentNodeContainer->getNodeID(), NodeNameStrategyUtils::getNodeNameFromFullPath(nameWithoutParent));

					parentNodeContainer = processor->getParentNodeContainer(parentNodeContainer);
					nodeName = parentPath;
				}
			}

			for (size_t i = 0; i < nodeDef->getNumChildNodes(); ++i)
			{
				const MR::NodeID childNodeID = nodeDef->getChildNodeID(i);

				if (childNodeID != MR::INVALID_NODE_ID)
				{
					MR::NodeDef* childNode = netDef->getNodeDef(childNodeID);
					collectNames(childNode);
				}
			}

			for (size_t i = 0; i < nodeDef->getNumInputCPConnections(); i++)
			{
				const MR::CPConnection* cpConnection = nodeDef->getInputCPConnection(i);

				if (cpConnection->m_sourceNodeID != MR::INVALID_NODE_ID)
				{
					MR::NodeDef* sourceNode = netDef->getNodeDef(cpConnection->m_sourceNodeID);

					if (!sourceNode->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
						collectNames(sourceNode);
				}
			}
		};

	collectNames(rootNodeDef);

	for (auto& nodeNamePair : nodeNameMap)
	{
		MR::NodeDef* nodeDef = netDef->getNodeDef(nodeNamePair.first);

		if (processor->isNodeBlendTree(nodeDef))
		{
			std::vector<ContainerNodeInfo*> blendTreesForNode = processor->getBlendTreesForNode(nodeNamePair.first);

			for (size_t i = 0; i < blendTreesForNode.size(); i++)
			{
				if (blendTreesForNode[i]->getName() == "")
				{
					char nodeNameBuffer[256];
					sprintf_s(nodeNameBuffer, "BlendTree_%d_%d", nodeNamePair.first, i);

					blendTreesForNode[i]->setName(nodeNameBuffer);
				}
			}
		}

		if (nodeNamePair.second == "")
		{
			char nodeNameBuffer[256];
			sprintf_s(nodeNameBuffer, "%s_%d", NodeProcessor::nodeTypeAsManifestName(nodeDef->getNodeTypeID()).c_str(), nodeNamePair.first);

			nodeNamePair.second = nodeNameBuffer;

			if (nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
				processor->getStateMachineForNode(nodeNamePair.first)->setName(nodeNameBuffer);
		}
		
		g_appLog->debugMessage(MsgLevel_Debug, "ReconstructParentChildNameStrategy::collectNodeNames: Associated node name '%s' for node ID %d (type=%d).\n", nodeNamePair.second.c_str(), nodeNamePair.first, netDef->getNodeDef(nodeNamePair.first)->getNodeTypeID());
	}

	return true;
}