#include "GraphNode.h"

namespace db
{
	tinyxml2::XMLElement* GraphNode::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);

		m_attributes.serialize(element);
		m_pins.serialize(element);

		return element;
	}
}
