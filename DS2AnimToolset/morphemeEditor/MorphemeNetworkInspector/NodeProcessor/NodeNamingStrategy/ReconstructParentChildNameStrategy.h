#pragma once
#include "NodeNamingStrategy.inl"

class ReconstructParentChildNameStrategy : public NodeNamingStrategy
{
public:
	ReconstructParentChildNameStrategy() = default;

	virtual ~ReconstructParentChildNameStrategy() = default;
	virtual bool collectNodeNames(MR::NetworkDef* netDef, NodeProcessor* processor) override;
};