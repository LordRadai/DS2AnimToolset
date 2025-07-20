#include "ControlParameter.h"
#include "../mcnSerializer/mcnSerializer.h"

namespace db
{
	bool ControlParameter::isValid() const
	{
		return Node::isValid() && !m_type.empty() && m_dataPin.isValid();
	}

	tinyxml2::XMLElement* ControlParameter::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);

		m_dataPin.serialize(element);
	}

	tinyxml2::XMLElement* ControlParameterBool::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = ControlParameter::serialize(parent);
		
		mcnSerializer::createBoolElement(element, "DefaultBool", m_value);

		return element;
	}

	tinyxml2::XMLElement* ControlParameterInt::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = ControlParameter::serialize(parent);

		mcnSerializer::createIntElement(element, "DefaultInt", m_value);
		mcnSerializer::createIntElement(element, "Min", m_min);
		mcnSerializer::createIntElement(element, "Max", m_max);
		return element;
	}

	tinyxml2::XMLElement* ControlParameterVector3::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = ControlParameter::serialize(parent);
		mcnSerializer::createVector3Element(element, "DefaultVector3", m_value);
		mcnSerializer::createFloatElement(element, "Min", m_min);
		mcnSerializer::createFloatElement(element, "Max", m_max);
		return element;
	}

	tinyxml2::XMLElement* ControlParameterVector4::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = ControlParameter::serialize(parent);
		mcnSerializer::createQuatElement(element, "DefaultQuat", m_value);
		mcnSerializer::createFloatElement(element, "Min", m_min);
		mcnSerializer::createFloatElement(element, "Max", m_max);
		return element;
	}

	tinyxml2::XMLElement* ControlParameterQuaternion::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = ControlParameter::serialize(parent);
		mcnSerializer::createQuatElement(element, "DefaultQuaternion", m_value);
		mcnSerializer::createFloatElement(element, "Min", m_min);
		mcnSerializer::createFloatElement(element, "Max", m_max);
		return element;
	}
}