#include "NodeEditorBase.h"
#include "imnodes/imnodes.h"
#include "Registry/Registry.h"

namespace NodeEditor
{
	NodeEditorBase::NodeEditorBase()
		: m_currentGraph(nullptr)
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

	void NodeEditorBase::draw()
	{
		ImNodes::BeginNodeEditor();

		if (m_currentGraph)
			m_currentGraph->draw();

		ImNodes::EndNodeEditor();
	}
}