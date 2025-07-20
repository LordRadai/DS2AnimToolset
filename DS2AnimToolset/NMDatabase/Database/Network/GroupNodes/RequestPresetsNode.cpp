#include "RequestPresetsNode.h"

namespace db
{
	tinyxml2::XMLElement* RequestPresetsNode::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		
		m_requestPresets.serialize(element);

		return element;
	}
}
