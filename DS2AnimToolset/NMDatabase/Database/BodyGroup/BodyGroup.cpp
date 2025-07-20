#include "BodyGroup.h"

namespace db
{
	tinyxml2::XMLElement* BodyGroup::serialize(tinyxml2::XMLElement* parent)
	{
		return Node::serialize(parent);
	}
}