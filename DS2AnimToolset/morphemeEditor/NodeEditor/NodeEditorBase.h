#pragma once
#include <stack>
#include "Graph/Graph.h"

namespace NodeEditor
{
	class NodeEditorBase
	{
	protected:
		bool m_showStyleEditor;
		std::stack<Graph*> m_graphStack;
	public:
		NodeEditorBase();
		virtual ~NodeEditorBase();

		virtual bool initialise();
		virtual void shutdown();
		virtual void update(float dt);
		virtual void draw();

		Graph* getCurrentGraph() const { return m_graphStack.top(); }
		void pushGraph(Graph* graph) { m_graphStack.push(graph); }
		void popGraph();

		void styleEditor();
	private:
		void initStyle();
	};
}