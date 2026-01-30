#include "BlendTree.h"
#include "NodeEditor/NodeEditorBase.h"

namespace NodeEditor
{
	BlendTree::BlendTree(NodeEditorBase* editor, Graph* parent, const std::string& name) : Graph(editor, parent, name)
	{
	}

	BlendTree::~BlendTree()
	{
	}

	void BlendTree::draw()
	{
		Graph::draw();

		for (Link* link : m_links)
			link->draw();
	}
}
