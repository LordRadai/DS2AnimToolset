#include "LayersNode.h"

namespace db
{
	tinyxml2::XMLElement* LayersNode::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		
		m_layers.serialize(element);

		return element;
	}
}
