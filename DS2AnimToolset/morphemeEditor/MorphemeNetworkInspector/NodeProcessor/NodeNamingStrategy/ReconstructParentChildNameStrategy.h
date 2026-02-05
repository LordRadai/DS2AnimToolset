#pragma once
#include "NodeNamingStrategy.inl"

class ReconstructParentChildNameStrategy : public NodeNamingStrategy
{
public:
	ReconstructParentChildNameStrategy() = default;

	virtual ~ReconstructParentChildNameStrategy() = default;
	virtual bool collectNodeNames(MR::NetworkDef* netDef, const std::map<MR::NodeID, MR::NodeDef*>& blendTreeNodes, std::map<MR::NodeID, std::string>& nodeNameMap, std::map<MR::NodeID, std::string>& blendTreeNodeNameMap) override;
};