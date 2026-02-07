#include "BlendTree.h"

#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	BlendTree::BlendTree(Editor* editor, Graph* parent, const std::string& name, Node* graphNode, int graphID) : Graph(editor, parent, name, graphNode, graphID), m_resultNode(nullptr)
	{
		m_resultNode = new BlendTreeOutputNode(editor, this);
		m_resultNode->setPosition(900.0f, 500.0f);

		m_controlParametersNode = new ControlParametersNode(editor, "Control Parameters");
		m_controlParametersNode->setPosition(100.f, 900.f);
		m_controlParametersNode->updateOutputPins();

		m_passDownPinsNode = new PassDownPinsNode(editor, this, -1, "Pass Down Pins");
		m_passDownPinsNode->setPosition(100.f, 100.f);
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

		if (m_passDownPins.size() > 0)
			m_passDownPinsNode->draw();

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

		if (m_passDownPins.size() > 0)
		{
			ImVec2 passDownPinsNodePos = ImNodes::GetNodeGridSpacePos(m_passDownPinsNode->getID());
			m_passDownPinsNode->setPosition(passDownPinsNodePos.x, passDownPinsNodePos.y);
		}
	}

	void BlendTree::setControlParamsNodePosition(float x, float y)
	{
		m_controlParametersNode->setPosition(x, y);
	}

	void BlendTree::setPassDownPinsNodePosition(float x, float y)
	{
		m_passDownPinsNode->setPosition(x, y);
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

	Node* BlendTree::getNodeConnectedToOutput() const
	{
		if (m_resultNode->getNumInputPins() > 0)
		{
			Pin* inputPin = m_resultNode->getInputPin(0);

			for (Link* link : m_links)
			{
				if (link->getInputPin() == inputPin)
					return link->getOutputPin()->getParentNode();
			}
		}

		return nullptr;
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

	bool BlendTree::hasConnectionBetween(Pin* outputPin, Pin* inputPin) const
	{
		for (Link* link : m_links)
		{
			if (link->getOutputPin() == outputPin && link->getInputPin() == inputPin)
				return true;
		}

		return false;
	}
}
