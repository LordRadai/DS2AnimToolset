#include "BlendTree.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	BlendTree::BlendTree(NodeEditor* editor, Graph* parent, const std::string& name) : Graph(editor, parent, name)
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
