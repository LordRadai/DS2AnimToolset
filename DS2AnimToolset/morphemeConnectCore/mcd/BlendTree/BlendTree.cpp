#include "BlendTree.h"
#include "mcd/Network/Network.h"
#include "mcd/Pin/FunctionalPin.h"
#include "mcu/Log.h"

namespace mcd
{
	BlendTree::BlendTree(db::CompositeAttribute* parent, std::string name) : Graph(parent, "BlendTree", name),
		m_blendTreeNodes(std::make_unique<db::TypedNodeContainer<mcd::BlendTreeNode>>(this, "BlendTreeNodes")),
		m_controlParamXPos(std::make_unique<db::FloatAttribute>(this, "ControlParamXPosition", -100.0f)),
		m_controlParamYPos(std::make_unique<db::FloatAttribute>(this, "ControlParamYPosition", -100.0f)),
		m_controlParamWidth(std::make_unique<db::FloatAttribute>(this, "ControlParamWidth", 100.0f)),
		m_controlParamHeight(std::make_unique<db::FloatAttribute>(this, "ControlParamHeight", 200.0f)),
		m_emittedControlParamXPos(std::make_unique<db::FloatAttribute>(this, "EmittedControlParamXPosition", 100.0f)),
		m_emittedControlParamYPos(std::make_unique<db::FloatAttribute>(this, "EmittedControlParamYPosition", 100.0f)),
		m_emittedControlParamWidth(std::make_unique<db::FloatAttribute>(this, "EmittedControlParamWidth", 100.0f)),
		m_emittedControlParamHeight(std::make_unique<db::FloatAttribute>(this, "EmittedControlParamHeight", 200.0f)),
		m_outputPinXPos(std::make_unique<db::FloatAttribute>(this, "OutputPinXPos", 100.0f)),
		m_outputPinYPos(std::make_unique<db::FloatAttribute>(this, "OutputPinYPos", 500.0f)),
		m_outputPinWidth(std::make_unique<db::FloatAttribute>(this, "OutputPinWidth", 100.0f)),
		m_outputPinHeight(std::make_unique<db::FloatAttribute>(this, "OutputPinHeight", 200.0f))
	{
		addAttribute(m_blendTreeNodes.get());
		addAttribute(m_controlParamXPos.get());
		addAttribute(m_controlParamYPos.get());
		addAttribute(m_controlParamWidth.get());
		addAttribute(m_controlParamHeight.get());
		addAttribute(m_emittedControlParamXPos.get());
		addAttribute(m_emittedControlParamYPos.get());
		addAttribute(m_emittedControlParamWidth.get());
		addAttribute(m_emittedControlParamHeight.get());
		addAttribute(m_outputPinXPos.get());
		addAttribute(m_outputPinYPos.get());
		addAttribute(m_outputPinWidth.get());
		addAttribute(m_outputPinHeight.get());
	}

	mcd::Pin* BlendTree::getPin(int idx)
	{
		if (idx == 0)
			return getResultPin();

		return nullptr;
	}

	void BlendTree::getFreePosition(float& x, float& y)
	{
		x = 10.0f;
		y = 0.0f;

		float maxX = 0.0f;
		float maxY = 0.0f;

		for (size_t i = 0; i < m_blendTreeNodes->size(); i++)
		{
			BlendTreeNode* node = m_blendTreeNodes->getNode(i);

			const float nodeX = node->getXPos();
			const float nodeY = node->getYPos();

			if (nodeX > maxX)
				maxX = nodeX;

			if (nodeY > maxY)
				maxY = nodeY;
		}

		x = 10.0f;
		y = maxY + 100.0f;
	}

	uint32_t BlendTree::getNumNodesOfType(const std::string& type) const
	{
		uint32_t count = 0;

		for (size_t i = 0; i < m_blendTreeNodes->size(); i++)
		{
			mcd::BlendTreeNode* node = m_blendTreeNodes->getNode(i);

			if (node->getNodeType() == type)
				count++;
		}

		return count;
	}

	mcd::PassDownPin* BlendTree::getResultPin()
	{
		if (hasParentNode<mcd::Network>())
		{
			mcd::Network* network = dynamic_cast<mcd::Network*>(getParentNode());

			return network->getResultPin();
		}
		
		if (hasParentNode<mcd::BlendTreeNode>())
		{
			mcd::BlendTreeNode* btNode = dynamic_cast<mcd::BlendTreeNode*>(getParentNode());

			mcd::Pin* pin = btNode->getPins()->find("Result");

			if (pin->isOfType<mcd::PassDownPin>())
				return dynamic_cast<mcd::PassDownPin*>(pin);
		}

		return nullptr;
	}

	mcd::FunctionalPin* BlendTree::getFunctionalPinConnectedToOutput()
	{
		PassDownPin* resultPin = getResultPin();
		Pin* connectedPin = getConnectedPin(resultPin);

		if (connectedPin)
			return dynamic_cast<mcd::FunctionalPin*>(connectedPin);

		return nullptr;
	}

	mcd::GraphNode* BlendTree::getNodeConnectedToOutput()
	{
		PassDownPin* resultPin = getResultPin();
		Pin* connectedPin = getConnectedPin(resultPin);

		if (!connectedPin)
			return nullptr;

		if (connectedPin->hasParentNode<GraphNode>())
			return dynamic_cast<GraphNode*>(connectedPin->getParentNode());
	}

	mcd::GraphNode* BlendTree::getNodeDirectlyConnectedToOutput()
	{
		PassDownPin* resultPin = getResultPin();
		Pin* connectedPin = getDirectlyConnectedPin(resultPin);

		if (!connectedPin)
			return nullptr;

		if (connectedPin->hasParentNode<GraphNode>())
			return dynamic_cast<GraphNode*>(connectedPin->getParentNode());
	}

	mcd::Pin* BlendTree::getDirectlyConnectedPin(mcd::Pin* pin)
	{
		for (size_t i = 0; i < m_flowEdges->size(); i++)
		{
			FlowEdge* edge = m_flowEdges->getNode(i);

			if (edge->getSourcePin() == pin)
				return edge->getDestinationPin();
			if (edge->getDestinationPin() == pin)
				return edge->getSourcePin();
		}

		return nullptr;
	}

	mcd::Pin* BlendTree::getConnectedPin(mcd::Pin* pin)
	{
		for (size_t i = 0; i < m_flowEdges->size(); i++)
		{
			FlowEdge* edge = m_flowEdges->getNode(i);

			if (edge->getSourcePin() == pin || edge->getDestinationPin() == pin)
			{
				std::vector<mcd::Pin*> connectedPins;
				edge->getOtherEnd(connectedPins, pin);

				if (connectedPins.size() == 1)
					return connectedPins[0];

				if (connectedPins.size() > 1)
				{
					mcu::logWarning("BlendTree::getConnectedPin called on a pin with multiple connections");
					return nullptr;
				}

				return nullptr;
			}
		}
	}
}
