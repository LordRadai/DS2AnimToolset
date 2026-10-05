#pragma once
#include "GraphLayouterStrategy.inl"

/*
* Same layout as the .mcn generator (Tools/mcnGen/layout.py):
*  - blend trees: the output on the right; each node's column is its longest path to the output, so every input sits
*    left of the node it feeds; within a column inputs keep their pin order (input 1 above input 2); the control
*    parameters node goes before the leftmost column.
*  - state machines: states on a grid (cell = largest state + margin), the assignment of states to cells optimised
*    against transition crossings, transitions running through other states and transition length; active state
*    nodes sit in a column on the left.
* Both work on the built editor graph (nodes and links), so they run after all connections exist.
*/
class ColumnBlendTreeLayouterStrategy : public GraphLayouterStrategy
{
public:
	ColumnBlendTreeLayouterStrategy() = default;
	virtual ~ColumnBlendTreeLayouterStrategy() = default;

	virtual bool setLayout(NodeEditor::Graph* graph, MR::NodeDef* graphNodeDef, std::vector<MR::NodeDef*>& childNodes) override;
};