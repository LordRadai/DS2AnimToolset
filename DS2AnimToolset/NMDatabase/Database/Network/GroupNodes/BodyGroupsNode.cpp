#include "BodyGroupsNode.h"

namespace db
{
	tinyxml2::XMLElement* BodyGroupsNode::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		
		m_bodyGroups.serialize(element);

		return element;
	}
}
