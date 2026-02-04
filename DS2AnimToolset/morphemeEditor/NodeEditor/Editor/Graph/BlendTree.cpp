#include "BlendTree.h"

#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	BlendTree::BlendTree(Editor* editor, Graph* parent, const std::string& name, int graphNodeID) : Graph(editor, parent, name, graphNodeID), m_resultNode(nullptr)
	{
		m_resultNode = new BlendTreeOutputNode(editor, this);
		m_resultNode->setPosition(900.0f, 500.0f);
		m_controlParamsNodePos = ImVec2(100.0f, 800.0f);

		ControlParametersNode* controlParamsNode = editor->getControlParametersNode();

		if (controlParamsNode)
			controlParamsNode->setPosition(m_controlParamsNodePos.x, m_controlParamsNodePos.y);
	}

	BlendTree::~BlendTree()
	{
		delete m_resultNode;
	}

	void BlendTree::draw()
	{
		Graph::draw();

		m_resultNode->draw();

		ControlParametersNode* cpNode = m_ownerEditor->getControlParametersNode();

		if (cpNode)
			cpNode->draw();

		for (Link* link : m_links)
			link->draw();
	}

	void BlendTree::updateNodePositions()
	{
		Graph::updateNodePositions();

		ControlParametersNode* cpNode = m_ownerEditor->getControlParametersNode();

		if (cpNode)
		{
			ImVec2 cpNodePos = ImNodes::GetNodeGridSpacePos(cpNode->getID());
			cpNode->setPosition(cpNodePos.x, cpNodePos.y);
			m_controlParamsNodePos = cpNodePos;
		}

		ImVec2 outputNodePos = ImNodes::GetNodeGridSpacePos(m_resultNode->getID());
		m_resultNode->setPosition(outputNodePos.x, outputNodePos.y);
	}

	Node* BlendTree::createNode(int nodeID, const std::string& typeName, const std::string& name)
	{
		return Graph::createNode(nodeID, typeName, name);
	}

	Node* BlendTree::createNode(int nodeID, const std::string& typeName, const std::string& name, float x, float y)
	{
		return Graph::createNode(nodeID, typeName, name, x, y);
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
