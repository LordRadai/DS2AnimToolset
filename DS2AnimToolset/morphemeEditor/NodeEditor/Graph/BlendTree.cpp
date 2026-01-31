#include "BlendTree.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	BlendTree::BlendTree(NodeEditor* editor, Graph* parent, const std::string& name) : Graph(editor, parent, name), m_resultNode(nullptr)
	{
		m_resultNode = new BlendTreeOutputNode(editor, this);
		m_resultNode->createInputPin("Output");

		m_resultNode->setPosition(600.0f, 200.0f);
	}

	BlendTree::~BlendTree()
	{
		delete m_resultNode;
	}

	void BlendTree::draw()
	{
		Graph::draw();

		m_resultNode->draw();

		for (Link* link : m_links)
			link->draw();
	}

	Node* BlendTree::createNode(int nodeID, const std::string& typeName, const std::string& name)
	{
		return Graph::createNode(nodeID, typeName, name);
	}

	bool BlendTree::connectToOutput(Pin* outputPin)
	{
		if (m_resultNode)
		{
			outputPin->connectTo(m_resultNode->getInputPin(0));
			return true;
		}

		return false;
	}
}
