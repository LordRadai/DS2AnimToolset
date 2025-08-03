#include "Vector3Attribute.h"

namespace db
{
	std::string Vector3Attribute::getValueAsString() const
	{
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "(%f, %f, %f)", m_value.x, m_value.y, m_value.z);

		return std::string(buffer);
	}

	bool Vector3Attribute::readValueXML(int format, db::XMLElement* element)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("Vector3Attribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "vector3")
		{
			throw std::runtime_error("Vector3Attribute::readValueXML: Expected type 'vector3'.");
			return false;
		}

		if (!element->getDataAsVector3(m_value))
		{
			throw std::runtime_error("Vector3Attribute::readValueXML: Failed to read vector3 value from XML element.");
			return false;
		}
		return true;
	}

	bool Vector3Attribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->InsertNewChildElement("X")->SetText(m_value.x);
		m_xmlElement->InsertNewChildElement("Y")->SetText(m_value.y);
		m_xmlElement->InsertNewChildElement("Z")->SetText(m_value.z);
		return true;
	}
}