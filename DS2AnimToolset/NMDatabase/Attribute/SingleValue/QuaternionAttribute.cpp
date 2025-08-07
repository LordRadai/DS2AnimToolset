#include "QuaternionAttribute.h"

namespace db
{
	bool QuaternionAttribute::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("QuaternionAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "quaternion")
		{
			throw std::runtime_error("QuaternionAttribute::readValueXML: Expected type 'quaternion'.");
			return false;
		}

		if (!element->getDataAsQuat(m_value))
		{
			throw std::runtime_error("QuaternionAttribute::readValueXML: Failed to read quaternion value from XML element.");
			return false;
		}

		return true;
	}

	bool QuaternionAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->InsertNewChildElement("X")->SetText(m_value.x);
		m_xmlElement->InsertNewChildElement("Y")->SetText(m_value.y);
		m_xmlElement->InsertNewChildElement("Z")->SetText(m_value.z);
		m_xmlElement->InsertNewChildElement("W")->SetText(m_value.w);
		return true;
	}

	std::string QuaternionAttribute::getValueAsString() const
	{
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "(%f, %f, %f, %f)", m_value.x, m_value.y, m_value.z, m_value.w);

		return std::string(buffer);
	}
}
