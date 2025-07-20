#include "RequestsNode.h"

namespace db
{
	tinyxml2::XMLElement* RequestsNode::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		
		m_requests.serialize(element);
		
		return element;
	}
}
