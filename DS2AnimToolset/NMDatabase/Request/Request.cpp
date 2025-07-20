#include "Request.h"

namespace db
{
	tinyxml2::XMLElement* Request::serialize(tinyxml2::XMLElement* parent)
	{		
		return Node::serialize(parent);
	}
}
