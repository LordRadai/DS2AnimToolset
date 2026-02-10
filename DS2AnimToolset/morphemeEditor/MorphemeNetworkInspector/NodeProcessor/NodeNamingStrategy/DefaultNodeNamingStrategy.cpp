#include <functional>

#include "DefaultNodeNamingStrategy.h"

#include "Utils/Utils.h"

#include "RLog/RLog.h"
#include "extern.h"

#include "../NodeProcessor.h"

bool DefaultNodeNamingStrategy::collectNodeNames(MR::NetworkDef* netDef, NodeProcessor* processor)
{
	std::map<MR::NodeID, std::string> nodeNameMap = processor->getNodeNameMap();
	nodeNameMap.clear();

	MR::NodeDef* rootNodeDef = netDef->getNodeDef(netDef->getRootNodeID());

	std::function<void(MR::NodeDef*)> collectNames;
	collectNames = [&](MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
				return;

			std::string nodeName = netDef->getNodeNameFromNodeID(nodeDef->getNodeID());

			if (processor->isNodeBlendTree(nodeDef))
			{
				std::vector<ContainerNodeInfo*> blendTreesForNode = processor->getBlendTreesForNode(nodeDef->getNodeID());

				for (size_t i = 0; i < blendTreesForNode.size(); i++)
				{
					std::string blendTreeName = NodeNameStrategyUtils::getBlendTreeNodeNameAtLayer(nodeName, i);

					g_appLog->debugMessage(MsgLevel_Debug, "DefaultNodeNamingStrategy::collectNodeNames: Registering blend tree node %d with name '%s'.\n", nodeDef->getNodeID(), blendTreeName.c_str());
					
					blendTreesForNode[i]->setName(blendTreeName);
				}	
			}

			g_appLog->debugMessage(MsgLevel_Debug, "DefaultNodeNamingStrategy::collectNodeNames: Registering node %d with name '%s'.\n", nodeDef->getNodeID(), nodeName.c_str());
			processor->registerNodeName(nodeDef->getNodeID(), NodeNameStrategyUtils::getNodeNameFromFullPath(nodeName));

			if (nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
				processor->registerStateMachineName(nodeDef->getNodeID(), NodeNameStrategyUtils::getNodeNameFromFullPath(nodeName));

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

	return true;
}