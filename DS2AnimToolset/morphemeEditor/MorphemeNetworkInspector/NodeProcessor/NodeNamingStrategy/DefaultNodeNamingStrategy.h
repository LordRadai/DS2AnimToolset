#pragma once
#include "NodeNamingStrategy.inl"

class DefaultNodeNamingStrategy : public NodeNamingStrategy
{
public:
	DefaultNodeNamingStrategy() = default;

	virtual ~DefaultNodeNamingStrategy() = default;
	virtual bool collectNodeNames(MR::NetworkDef* netDef, NodeProcessor* processor) override;
};