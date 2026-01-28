#include "NodeEditorBase.h"
#include "imnodes/imnodes.h"
#include "Registry/Registry.h"
#include "extern.h"
#include "RLog/RLog.h"

namespace NodeEditor
{
	NodeEditorBase::NodeEditorBase()
	{
	}

	NodeEditorBase::~NodeEditorBase()
	{
		shutdown();
	}

	bool NodeEditorBase::initialise()
	{
		if (!ImNodes::CreateContext())
			return false;

		Registry::getInstance();

		return true;
	}

	void NodeEditorBase::shutdown()
	{
		Registry::destroyInstance();
		ImNodes::DestroyContext();
	}

	void NodeEditorBase::update(float dt)
	{
		int hoveredNodeId = -1;

		if (ImNodes::IsNodeHovered(&hoveredNodeId))
		{
			Graph* currentGraph = getCurrentGraph();

			Registry* registry = Registry::getInstance();
			Node* hoveredNode = static_cast<Node*>(registry->findEntity(hoveredNodeId));

			if (hoveredNode && hoveredNode->hasSubGraph() && ImGui::IsMouseDoubleClicked(0))
				pushGraph(hoveredNode->getSubGraph());
		}
		else if (ImGui::IsMouseDoubleClicked(1))
		{
			popGraph();
		}
	}

	void NodeEditorBase::draw()
	{
		ImNodes::BeginNodeEditor();

		Graph* currentGraph = getCurrentGraph();

		if (currentGraph)
			currentGraph->draw();

		ImNodes::EndNodeEditor();	
	}

	void NodeEditorBase::popGraph()
	{
		if (m_graphStack.size() > 1)
			m_graphStack.pop();
	}
}