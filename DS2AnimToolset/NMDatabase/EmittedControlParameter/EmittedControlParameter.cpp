#include "EmittedControlParameter.h"
#include "../mcnSerializer/mcnSerializer.h"

namespace db
{
	tinyxml2::XMLElement* EmittedControlParameter::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		element->SetName("EmittedControlParameter");

		if (m_controlParameter)
		{
			tinyxml2::XMLElement* controlParamElement = m_controlParameter->serialize(element);

			mcnSerializer::createPointerElement(element, "ControlParameter", m_controlParameter->getQualifiedName());
		}

		return element;
	}
}
