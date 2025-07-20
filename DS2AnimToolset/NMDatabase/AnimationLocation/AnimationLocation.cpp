#include "AnimationLocation.h"
#include "../mcnSerializer/mcnSerializer.h"

namespace db
{
	tinyxml2::XMLElement* AnimationLocation::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* newElement = Node::serialize(parent);

		newElement->SetName("AnimationLocation");
		
		mcnSerializer::createStringElement(newElement, "SourceDir", m_sourceDir);
		mcnSerializer::createStringElement(newElement, "MarkupDir", m_markupDir);

		return newElement;
	}
}