#include "MessagePresetsGroupNode.h"

namespace db
{
	tinyxml2::XMLElement* MessagePresetsGroupNode::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);

		m_messagePresets.serialize(element);

		return element;
	}
}
