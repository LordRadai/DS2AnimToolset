#pragma once
#include <vector>
#include "Graph/Graph.h"

namespace NodeEditor
{
	class NodeEditorBase
	{
	protected:
		Graph* m_currentGraph;
	public:
		NodeEditorBase();
		virtual ~NodeEditorBase();

		virtual bool initialise();
		virtual void shutdown();
		virtual void draw();

		Graph* getCurrentGraph() const { return m_currentGraph; }
		void setCurrentGraph(Graph* graph) { m_currentGraph = graph; }
	};
}