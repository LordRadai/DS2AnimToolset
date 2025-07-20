#include "Attribute.h"
#include "mcnSerializer/mcnSerializer.h"
#include "../Request/Request.h"
#include "../ControlParameter/ControlParameter.h"

namespace db
{
	bool Attribute::isValid() const
	{
		if (!Node::isValid())
			return false;

		if (m_type.empty())
			return false;

		return true;
	}

	tinyxml2::XMLElement* AttributePlaceholder::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Attribute::serialize(parent);
		tinyxml2::XMLElement* attribEntry = mcnSerializer::createNodeContainerElement(element, "AttributeEntry");

		m_attribute->serialize(attribEntry);

		return element;
	}

	tinyxml2::XMLElement* AnimationSetAttribute::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Attribute::serialize(parent);

		m_attributePlaceholders.serialize(element);

		return element;
	}

	tinyxml2::XMLElement* BoolAttribute::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Attribute::serialize(parent);

		mcnSerializer::createBoolElement(element, "Value", m_value);

		return element;
	}

	tinyxml2::XMLElement* FloatAttribute::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Attribute::serialize(parent);

		mcnSerializer::createFloatElement(element, "Value", m_value);

		return element;
	}

	tinyxml2::XMLElement* IntAttribute::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Attribute::serialize(parent);

		mcnSerializer::createIntElement(element, "Value", m_value);

		return element;
	}

	tinyxml2::XMLElement* ControlParameterAttribute::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Attribute::serialize(parent);

		mcnSerializer::createPointerElement(element, "ControlParameterDataPin", m_controlParameter->getDataPin()->getQualifiedName());

		return element;
	}

	tinyxml2::XMLElement* RequestAttribute::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Attribute::serialize(parent);
		mcnSerializer::createPointerElement(element, "RequestEntry", m_request->getQualifiedName());
		
		return element;
	}

	tinyxml2::XMLElement* RefAttribute::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Attribute::serialize(parent);
		mcnSerializer::createIntElement(element, "RefKind", m_refKind);

		return element;
	}
}