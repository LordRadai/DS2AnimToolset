#include "RequestPreset.h"
#include "../mcnSerializer/mcnSerializer.h"

namespace db
{
	tinyxml2::XMLElement* RequestPreset::serialize(tinyxml2::XMLElement* parent)
	{
		return Node::serialize(parent);
	}
}