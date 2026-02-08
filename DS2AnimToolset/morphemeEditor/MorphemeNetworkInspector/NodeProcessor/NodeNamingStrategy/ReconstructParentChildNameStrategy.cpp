#include <functional>

#include "ReconstructParentChildNameStrategy.h"

#include "Utils/Utils.h"

#include "RLog/RLog.h"
#include "extern.h"

#include "../NodeProcessor.h"

bool ReconstructParentChildNameStrategy::collectNodeNames(MR::NetworkDef* netDef, const std::map<MR::NodeID, std::vector<MR::NodeDef*>>& blendTreeChildren, const std::map<MR::NodeID, std::vector<MR::NodeDef*>>& smChildren, std::map<MR::NodeID, std::string>& nodeNameMap, std::map<MR::NodeID, std::string>& blendTreeNodeNameMap)
{
	nodeNameMap.clear();
	blendTreeNodeNameMap.clear();

	std::map<MR::NodeID, MR::NodeDef*> blendTreeNodes;
	for (const auto& nodeContainerPair : blendTreeChildren)
	{
		MR::NodeDef* nodeDef = netDef->getNodeDef(nodeContainerPair.first);
		blendTreeNodes[nodeContainerPair.first] = nodeDef;
	}

	MR::NodeDef* rootNodeDef = netDef->getNodeDef(netDef->getRootNodeID());

	std::function<void(MR::NodeDef*)> collectNames;
	collectNames = [&](MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
				return;

			std::string nodeName = netDef->getNodeNameFromNodeID(nodeDef->getNodeID());

			if (NodeNameStrategyUtils::isNodeBlendTreeOutput(nodeDef, blendTreeNodes))
				NodeNameStrategyUtils::registerNodeName(blendTreeNodeNameMap, nodeDef->getNodeID(), NodeNameStrategyUtils::getNodeNameFromFullPath(nodeName));

			NodeNameStrategyUtils::registerNodeName(nodeNameMap, nodeDef->getNodeID(), NodeNameStrategyUtils::getNodeNameFromFullPath(nodeName));

			if (!nodeName.empty())
			{
				MR::NodeDef* parentNodeContainer = NodeNameStrategyUtils::getParentNodeContainer(nodeDef, blendTreeChildren, smChildren);

				while (parentNodeContainer && parentNodeContainer->getNodeID() != netDef->getRootNodeID())
				{
					const std::string parentPath = NodeNameStrategyUtils::getParentNodeNameFromFullPath(nodeName);
					std::string nameWithoutParent = NodeNameStrategyUtils::getNodeNameFromFullPath(parentPath);

					if (NodeNameStrategyUtils::isNodeBlendTreeOutput(parentNodeContainer, blendTreeNodes))
					{
						NodeNameStrategyUtils::registerNodeName(blendTreeNodeNameMap, parentNodeContainer->getNodeID(), nameWithoutParent);
						nameWithoutParent = "";
					}

					NodeNameStrategyUtils::registerNodeName(nodeNameMap, parentNodeContainer->getNodeID(), nameWithoutParent);

					parentNodeContainer = NodeNameStrategyUtils::getParentNodeContainer(parentNodeContainer, blendTreeChildren, smChildren);
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
		if (nodeNamePair.second == "")
		{
			MR::NodeDef* nodeDef = netDef->getNodeDef(nodeNamePair.first);

			char nodeNameBuffer[256];
			sprintf_s(nodeNameBuffer, "%s_%d", NodeProcessor::nodeTypeAsManifestName(nodeDef->getNodeTypeID()).c_str(), nodeNamePair.first);

			nodeNamePair.second = nodeNameBuffer;
		}
		
		g_appLog->debugMessage(MsgLevel_Debug, "DefaultNodeNamingStrategy::collectNodeNames: Associated node name '%s' for node ID %d (type=%d).\n", nodeNamePair.second.c_str(), nodeNamePair.first, netDef->getNodeDef(nodeNamePair.first)->getNodeTypeID());
	}

	for (auto& blendTreeNodeNamePair : blendTreeNodeNameMap)
	{
		if (blendTreeNodeNamePair.second == "")
		{
			char nodeNameBuffer[256];
			sprintf_s(nodeNameBuffer, "BlendTree_%d", blendTreeNodeNamePair.first);

			blendTreeNodeNamePair.second = nodeNameBuffer;
		}

		g_appLog->debugMessage(MsgLevel_Debug, "DefaultNodeNamingStrategy::collectNodeNames: Associated blend tree node name '%s' for node ID %d.\n", blendTreeNodeNamePair.second.c_str(), blendTreeNodeNamePair.first);
	}

	return true;
}