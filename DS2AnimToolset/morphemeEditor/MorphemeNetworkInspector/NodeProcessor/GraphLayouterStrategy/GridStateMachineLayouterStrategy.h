#pragma once
#include "GraphLayouterStrategy.inl"

class GridStateMachineLayouterStrategy : public GraphLayouterStrategy
{
public:
	GridStateMachineLayouterStrategy() = default;
	virtual ~GridStateMachineLayouterStrategy() = default;

	virtual bool setLayout(NodeEditor::Graph* graph, MR::NodeDef* graphNodeDef, std::vector<MR::NodeDef*>& childNodes) override;
};