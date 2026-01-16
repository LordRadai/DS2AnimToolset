#include "GraphNode.h"

namespace mcd
{
	GraphNode::GraphNode(db::CompositeAttribute* parent, std::string name, std::string nodeName, float xPos, float yPos, float width, float height)
		: db::Node(parent, name, nodeName),
		m_graphEntry(std::make_unique<db::NodeContainer>(this, "GraphEntry")),
		m_attributes(std::make_unique<db::TypedNodeContainer<mcd::Attribute>>(this, "Attributes")),
		m_pins(std::make_unique<db::TypedNodeContainer<mcd::Pin>>(this, "Pins")),
		m_xPos(std::make_unique<db::FloatAttribute>(this, "XPosition", xPos)),
		m_yPos(std::make_unique<db::FloatAttribute>(this, "YPosition", yPos)),
		m_width(std::make_unique<db::FloatAttribute>(this, "Width", width)),
		m_height(std::make_unique<db::FloatAttribute>(this, "Height", height))
	{
		addAttribute(m_xPos.get());
		addAttribute(m_yPos.get());
		addAttribute(m_width.get());
		addAttribute(m_height.get());
	}

	void GraphNode::addMcdAttribute(mcd::Attribute* attribute)
	{
		m_attributes->add(attribute);

		if (findAttribute("Attributes") == nullptr)
			addAttribute(m_attributes.get());
	}

	void GraphNode::addPin(mcd::Pin* pin)
	{
		m_pins->add(pin);

		if (findAttribute("Pins") == nullptr)
			addAttribute(m_pins.get());
	}

	void GraphNode::setGraphEntry(db::Node* node)
	{
		removeAttribute(m_graphEntry.get());

		if (m_graphEntry->size())
			m_graphEntry->clearArray();

		m_graphEntry->add(node);

		addAttribute(m_graphEntry.get());
	}

	db::Node* GraphNode::getGraphEntryNode() const
	{
		if (m_graphEntry->size() == 0)
			return nullptr;

		return m_graphEntry->getNode(0);
	}
}
