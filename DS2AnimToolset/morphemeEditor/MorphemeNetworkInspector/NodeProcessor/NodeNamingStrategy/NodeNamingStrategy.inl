#pragma once
#include <string>
#include <map>

#include "morpheme/mrNetworkDef.h"

class NodeProcessor;

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
	virtual bool collectNodeNames(MR::NetworkDef* netDef, NodeProcessor* processor) = 0;
};