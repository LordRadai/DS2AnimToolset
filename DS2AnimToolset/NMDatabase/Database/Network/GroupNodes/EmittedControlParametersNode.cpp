#include "EmittedControlParametersNode.h"

namespace db
{
	tinyxml2::XMLElement* EmittedControlParametersNode::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		
		m_emittedControlParameters.serialize(element);

		return element;
	}
}
