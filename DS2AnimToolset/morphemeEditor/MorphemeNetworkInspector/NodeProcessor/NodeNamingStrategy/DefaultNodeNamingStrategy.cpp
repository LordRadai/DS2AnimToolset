#include <functional>

#include "DefaultNodeNamingStrategy.h"

#include "Utils/Utils.h"

#include "RLog/RLog.h"
#include "extern.h"

bool DefaultNodeNamingStrategy::collectNodeNames(MR::NetworkDef* netDef, const std::map<MR::NodeID, MR::NodeDef*>& blendTreeNodes, std::map<MR::NodeID, std::string>& nodeNameMap, std::map<MR::NodeID, std::string>& blendTreeNodeNameMap)
{
	nodeNameMap.clear();
	blendTreeNodeNameMap.clear();

	MR::NodeDef* rootNodeDef = netDef->getNodeDef(netDef->getRootNodeID());

	std::function<void(MR::NodeDef*)> collectNames;
	collectNames = [&](MR::NodeDef* nodeDef)
		{
			std::string nodeName = netDef->getNodeNameFromNodeID(nodeDef->getNodeID());

			if (NodeNameStrategyUtils::isNodeBlendTreeOutput(nodeDef, blendTreeNodes))
			{
				std::string blendTreeName = NodeNameStrategyUtils::getBlendTreeNodeName(nodeName);

				if (NodeNameStrategyUtils::isNodeStateNode(nodeDef))
					blendTreeName = NodeNameStrategyUtils::getStateNodeNameFromStringTable(netDef, nodeDef->getNodeID());

				NodeNameStrategyUtils::registerNodeName(blendTreeNodeNameMap, nodeDef->getNodeID(), blendTreeName);
			}

			NodeNameStrategyUtils::registerNodeName(nodeNameMap, nodeDef->getNodeID(), NodeNameStrategyUtils::getNodeNameFromFullPath(nodeName));

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

	for (const auto& nodeNamePair : nodeNameMap)
		g_appLog->debugMessage(MsgLevel_Debug, "DefaultNodeNamingStrategy::collectNodeNames: Associated node name '%s' for node ID %d.\n", nodeNamePair.second.c_str(), nodeNamePair.first);

	for (const auto& blendTreeNodeNamePair : blendTreeNodeNameMap)
		g_appLog->debugMessage(MsgLevel_Debug, "DefaultNodeNamingStrategy::collectNodeNames: Associated blend tree node name '%s' for node ID %d.\n", blendTreeNodeNamePair.second.c_str(), blendTreeNodeNamePair.first);

	return true;
}