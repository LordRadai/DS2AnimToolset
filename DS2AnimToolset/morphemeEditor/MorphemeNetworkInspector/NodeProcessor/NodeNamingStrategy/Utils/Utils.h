#pragma once
#include <string>
#include <map>
#include <vector>

#include "morpheme/mrNetworkDef.h"

namespace NodeNameStrategyUtils
{
	const std::string getStateNodeNameFromStringTable(MR::NetworkDef* netDef, MR::NodeID nodeID);
	const std::string getNodeNameFromFullPath(const std::string& name);
	const std::string getNodeNameWithParentFromFullPath(const std::string& name);
	const std::string getParentNodeNameFromFullPath(const std::string& name);
	const std::string getBlendTreeNodeName(const std::string& name);

	void registerNodeName(std::map<MR::NodeID, std::string>& nodeNameMap, MR::NodeID nodeID, const std::string& name);

	inline bool isNodeBlendTreeOutput(MR::NodeDef* nodeDef, const std::map<MR::NodeID, MR::NodeDef*>& blendTreeNodes)
	{
		return (blendTreeNodes.find(nodeDef->getNodeID()) != blendTreeNodes.end());
	}

	inline bool isNodeStateNode(MR::NodeDef* nodeDef)
	{
		if (nodeDef->getParentNodeDef())
			return nodeDef->getParentNodeDef()->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE);

		return false;
	}

	MR::NodeDef* getParentNodeContainer(MR::NodeDef* nodeDef, const std::map<MR::NodeID, std::vector<MR::NodeDef*>>& btChildren, const std::map<MR::NodeID, std::vector<MR::NodeDef*>>& smChildren);
}