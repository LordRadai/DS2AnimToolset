#pragma once
#include "GraphLayouterStrategy.inl"

class BTFanLayouterStrategy : public GraphLayouterStrategy
{
public:
	BTFanLayouterStrategy() = default;

	virtual ~BTFanLayouterStrategy() = default;
	virtual bool setLayout(NodeEditor::Graph* graph, MR::NodeDef* graphNodeDef, std::vector<MR::NodeDef*>& childNodes) override;
};