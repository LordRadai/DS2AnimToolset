#pragma once
#include <string>
#include <map>

#include "morpheme/mrNetworkDef.h"

class NodeNamingStrategy
{
public:
	NodeNamingStrategy() = default;
	virtual ~NodeNamingStrategy() = default;

	/*
	* \brief Collect all node names in the network and register them.
	* \param netDef The network definition to process.
	* \param blendTreeNodes Map of blend tree nodes in the network.
	* \param nodeNameMap Map to populate with node ID to node name associations.
	* \param blendTreeNodeNameMap Map to populate with blend tree node ID to blend tree name associations.
	*/
	virtual bool collectNodeNames(MR::NetworkDef* netDef, const std::map<MR::NodeID, std::vector<MR::NodeDef*>>& blendTreeChildren, const std::map<MR::NodeID, std::vector<MR::NodeDef*>>& smChildren, std::map<MR::NodeID, std::string>& nodeNameMap, std::map<MR::NodeID, std::string>& blendTreeNodeNameMap) = 0;
};