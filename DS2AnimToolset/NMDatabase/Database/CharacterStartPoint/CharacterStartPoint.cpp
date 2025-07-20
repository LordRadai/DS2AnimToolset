#include "CharacterStartPoint.h"

namespace db
{
	bool CharacterStartPoint::isValid() const
	{
		return m_animationSet != nullptr && m_animationSet->isValid() && Node::isValid();
	}

	tinyxml2::XMLElement* CharacterStartPoint::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		
		if (m_animationSet)
			tinyxml2::XMLElement* animSetElement = m_animationSet->serialize(element);

		return element;
	}
}