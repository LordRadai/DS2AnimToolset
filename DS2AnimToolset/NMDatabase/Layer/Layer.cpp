#include "Layer.h"

namespace db
{
	tinyxml2::XMLElement* Layer::serialize(tinyxml2::XMLElement* parent)
	{
		return Node::serialize(parent);
	}
}
