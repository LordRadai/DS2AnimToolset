#pragma once
#include <string>
#include <map>

#include "morpheme/mrNetworkDef.h"

namespace NodeNameStrategyUtils
{
	const std::string getNodeNameFromFullPath(const std::string& name);
	const std::string getNodeNameWithParentFromFullPath(const std::string& name);
	const std::string getBlendTreeNodeName(const std::string& name);

	void registerNodeName(std::map<MR::NodeID, std::string>& nodeNameMap, MR::NodeID nodeID, const std::string& name);

	inline bool isNodeBlendTreeOutput(MR::NodeDef* nodeDef, const std::map<MR::NodeID, MR::NodeDef*>& blendTreeNodes)
	{
		return (blendTreeNodes.find(nodeDef->getNodeID()) != blendTreeNodes.end());
	}
}