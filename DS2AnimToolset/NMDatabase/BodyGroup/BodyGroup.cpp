#include "BodyGroup.h"

namespace db
{
	tinyxml2::XMLElement* BodyGroup::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		element->SetName("BodyGroup");
		element->SetAttribute("name", m_name.c_str());

		return element;
	}
}