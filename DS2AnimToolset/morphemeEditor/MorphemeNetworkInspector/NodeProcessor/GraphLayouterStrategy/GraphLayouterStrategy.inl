#pragma once
#include <vector>

#include "NodeEditor/Editor/Graph/Graph.h"
#include "morpheme/mrNodeDef.h"

class GraphLayouterStrategy
{
public:
	GraphLayouterStrategy() = default;

	virtual ~GraphLayouterStrategy() = default;

	/*
	* \brief Set the layout of the given graph based on its child nodes.
	* \param graph The graph to layout.
	* \param childNodes The child node defs for the graph nodes.
	*/
	virtual bool setLayout(NodeEditor::Graph* graph, MR::NodeDef* graphNodeDef, std::vector<MR::NodeDef*>& childNodes) = 0;
};