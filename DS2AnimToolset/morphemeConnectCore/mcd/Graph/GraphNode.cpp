#include "GraphNode.h"
#include "Graph.h"
#include "mcd/Pin/FunctionalPin.h"
#include "mcu/Log.h"

namespace mcd
{
	GraphNode::GraphNode(db::CompositeAttribute* parent, std::string name, std::string nodeName, float xPos, float yPos, float width, float height)
		: mcd::AttributePinNodeBase(parent, name, nodeName),
		m_graphEntry(std::make_unique<db::NodeContainer>(this, "GraphEntry")),
		m_attributes(std::make_unique<db::TypedNodeContainer<mcd::Attribute>>(this, "Attributes")),
		m_pins(std::make_unique<db::TypedNodeContainer<mcd::Pin>>(this, "Pins")),
		m_xPos(std::make_unique<db::FloatAttribute>(this, "XPosition", xPos)),
		m_yPos(std::make_unique<db::FloatAttribute>(this, "YPosition", yPos)),
		m_width(std::make_unique<db::FloatAttribute>(this, "Width", width)),
		m_height(std::make_unique<db::FloatAttribute>(this, "Height", height)),
		m_ownerGraphs(std::make_unique<db::TypedNodeContainer<mcd::Graph>>(this, "OwnerGraphs"))
	{
		addAttribute(m_attributes.get());
		addAttribute(m_pins.get());
		addAttribute(m_xPos.get());
		addAttribute(m_yPos.get());
		addAttribute(m_width.get());
		addAttribute(m_height.get());
	}

	void GraphNode::setGraphEntry(db::Node* node)
	{
		removeAttribute(m_graphEntry.get());

		if (m_graphEntry->size())
			m_graphEntry->clearArray();

		m_graphEntry->add(node);

		insertAttribute(0, m_graphEntry.get());
	}

	db::Node* GraphNode::getGraphEntryNode() const
	{
		if (m_graphEntry->size() == 0)
			return nullptr;

		return m_graphEntry->getNode(0);
	}

	mcd::Graph* GraphNode::getGraph()
	{
		if (m_ownerGraphs->size() == 0)
			return nullptr;
		
		return m_ownerGraphs->getNode(0);
	}

	void GraphNode::setOwnerGraph(mcd::Graph* graph)
	{
		if (m_ownerGraphs->size())
			m_ownerGraphs->clearArray();

		if (graph)
			m_ownerGraphs->add(graph);
	}

	bool GraphNode::getInputFunctionalPins(InputPinQueryType queryType, std::vector<mcd::FunctionalPin*>* outPins)
	{
		if (outPins == nullptr)
			return false;

		for (uint32_t i = 0; i < getPinCount(); ++i)
		{
			mcd::Pin* pin = getPin(i);

			if (!pin->isInput() || !pin->isOfType<FunctionalPin>())
				continue;

			FunctionalPin* funcPin = pin->asFunctionalPin();

			bool matchesCriteria = false;

			switch (queryType)
			{
			case mcd::GraphNode::InputPinQueryType::kPassThroughEnabled:
				matchesCriteria = funcPin->isPassThroughEnabled();
				break;
			case mcd::GraphNode::InputPinQueryType::kConnectedAndPassThroughEnabled:
				matchesCriteria = funcPin->isPassThroughEnabled() && funcPin->hasIncidentEdge();
				break;
			case mcd::GraphNode::InputPinQueryType::kAll:
				matchesCriteria = true;
				break;
			case mcd::GraphNode::InputPinQueryType::kConnected:
				matchesCriteria = funcPin->hasIncidentEdge();
				break;
			default:
				mcu::logError("Unknown InputPinQueryType in GraphNode::getInputFunctionalPins\n");
				return false;
			}

			if (matchesCriteria)
				outPins->push_back(funcPin);
		}
	}
}
