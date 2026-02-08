#pragma once
#include "NodeNamingStrategy.inl"

class ReconstructParentChildNameStrategy : public NodeNamingStrategy
{
public:
	ReconstructParentChildNameStrategy() = default;

	virtual ~ReconstructParentChildNameStrategy() = default;
	virtual bool collectNodeNames(MR::NetworkDef* netDef, const std::map<MR::NodeID, std::vector<MR::NodeDef*>>& blendTreeChildren, const std::map<MR::NodeID, std::vector<MR::NodeDef*>>& smChildren, std::map<MR::NodeID, std::string>& nodeNameMap, std::map<MR::NodeID, std::string>& blendTreeNodeNameMap) override;
};