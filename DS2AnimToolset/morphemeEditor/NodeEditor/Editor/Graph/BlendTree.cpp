#include "BlendTree.h"

#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	BlendTree::BlendTree(Editor* editor, Graph* parent, const std::string& name, int graphNodeID) : Graph(editor, parent, name, graphNodeID), m_resultNode(nullptr)
	{
		m_resultNode = new BlendTreeOutputNode(editor, this);
		m_resultNode->setPosition(900.0f, 500.0f);
		m_controlParametersNode = new ControlParametersNode(editor, "ControlParameters");
		m_controlParametersNode->setPosition(100.f, 400.f);
		m_controlParametersNode->updateOutputPins();
	}

	BlendTree::~BlendTree()
	{
		delete m_resultNode;
	}

	void BlendTree::draw()
	{
		Graph::draw();

		m_resultNode->draw();
		m_controlParametersNode->draw();

		for (Link* link : m_links)
			link->draw();
	}

	void BlendTree::updateNodePositions()
	{
		Graph::updateNodePositions();

		ImVec2 cpNodePos = ImNodes::GetNodeGridSpacePos(m_controlParametersNode->getID());
		m_controlParametersNode->setPosition(cpNodePos.x, cpNodePos.y);

		ImVec2 outputNodePos = ImNodes::GetNodeGridSpacePos(m_resultNode->getID());
		m_resultNode->setPosition(outputNodePos.x, outputNodePos.y);
	}

	void BlendTree::setControlParamsNodePosition(float x, float y)
	{
		m_controlParametersNode->setPosition(x, y);
	}

	Node* BlendTree::createNode(int nodeID, const std::string& typeName, const std::string& name)
	{
		return Graph::createNode(nodeID, typeName, name);
	}

	Node* BlendTree::createNode(int nodeID, const std::string& typeName, const std::string& name, float x, float y)
	{
		return Graph::createNode(nodeID, typeName, name, x, y);
	}

	DataPin* BlendTree::getControlParameterDataPin(const std::string& paramName) const
	{
		return m_controlParametersNode->getOutputDataPin(paramName);
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
