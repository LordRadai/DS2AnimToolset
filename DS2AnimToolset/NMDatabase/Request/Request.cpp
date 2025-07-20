#include "Request.h"

namespace db
{
	tinyxml2::XMLElement* Request::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		element->SetName("Request");
		
		return element;
	}
}
