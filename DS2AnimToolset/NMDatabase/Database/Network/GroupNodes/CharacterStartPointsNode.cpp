#include "CharacterStartPointsNode.h"

namespace db
{
	tinyxml2::XMLElement* CharacterStartPointsNode::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		
		m_characterStartPoints.serialize(element);

		return element;
	}
}
