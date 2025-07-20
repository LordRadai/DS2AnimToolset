#include "AnimationSetsNode.h"

namespace db
{
	tinyxml2::XMLElement* AnimationSetsNode::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		
		m_animationSets.serialize(element);

		return element;
	}
}
