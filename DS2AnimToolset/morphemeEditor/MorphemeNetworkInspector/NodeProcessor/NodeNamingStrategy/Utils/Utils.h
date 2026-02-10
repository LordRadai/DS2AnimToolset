#pragma once
#include <string>
#include <map>
#include <vector>

#include "morpheme/mrNetworkDef.h"

union BlendTreeID;
class ContainerNodeInfo;

namespace NodeNameStrategyUtils
{
	const std::string getStateNodeNameFromStringTable(MR::NetworkDef* netDef, MR::NodeID nodeID);
	const std::string getNodeNameFromFullPath(const std::string& name);
	const std::string getNodeNameWithParentFromFullPath(const std::string& name);
	const std::string getParentNodeNameFromFullPath(const std::string& name);
	const std::string getBlendTreeNodeNameAtLayer(const std::string& name, int layerIdx);
}