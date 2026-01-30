#pragma once
#include <stack>
#include "Graph/Graph.h"
#include "Registry/Registry.h"

namespace NodeEditor
{
	struct StyleSettings
	{
		float nodeMinContentHeight;
		float nodeMinWidth;
		float nodePinSpacing;

		StyleSettings();
	};

	class NodeEditorBase
	{
	protected:
		StyleSettings m_styleSettings;
		bool m_showStyleEditor;
		std::stack<Graph*> m_graphStack;
		Registry* m_registry;
	public:
		NodeEditorBase();
		virtual ~NodeEditorBase();

		virtual bool initialise();
		virtual void shutdown();
		virtual void update(float dt);
		virtual void draw();

		StyleSettings& getStyleSettings() { return m_styleSettings; }
		Registry* getRegistry() const { return m_registry; }

		Graph* getCurrentGraph() const { return m_graphStack.top(); }
		void pushGraph(Graph* graph) { m_graphStack.push(graph); }
		void popGraph();

		void styleEditor();
	private:
		void initStyle();
	};
}