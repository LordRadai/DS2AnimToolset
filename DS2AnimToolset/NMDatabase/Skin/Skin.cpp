#include "Skin.h"
#include "../mcnSerializer/mcnSerializer.h"

namespace db
{
	bool Skin::isValid() const
	{
		return true;
	}

	tinyxml2::XMLElement* Skin::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* skinElement = Node::serialize(parent);
		skinElement->SetName("Skin");

		mcnSerializer::createStringElement(skinElement, "Path", m_path);

		return skinElement
	}
}