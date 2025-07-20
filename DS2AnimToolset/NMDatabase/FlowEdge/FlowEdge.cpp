#include "FlowEdge.h"
#include "../NodeDef/NodeDef.h"
#include "../mcnSerializer/mcnSerializer.h"

namespace db
{
	tinyxml2::XMLElement* FlowEdge::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);

		mcnSerializer::createPointerElement(element, "Source", m_source->getQualifiedName());
		mcnSerializer::createPointerElement(element, "Destination", m_destination->getQualifiedName());

		return element;
	}
}