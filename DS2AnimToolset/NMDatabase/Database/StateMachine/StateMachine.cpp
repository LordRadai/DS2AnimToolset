#include "StateMachine.h"
#include "mcnSerializer/mcnSerializer.h"

namespace db
{
	bool StateMachine::isValid() const
	{
		if (!Node::isValid())
			return false;

		if (m_defaultState == nullptr)
			return false;

		return true;
	}

	tinyxml2::XMLElement* StateMachine::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);

		mcnSerializer::createFloatElement(element, "PanX", m_panX);
		mcnSerializer::createFloatElement(element, "PanY", m_panY);
		m_stateMachineNodes.serialize(element);
		mcnSerializer::createPointerElement(element, "DefaultState", m_defaultState->getQualifiedName());

		return element;
	}
}
