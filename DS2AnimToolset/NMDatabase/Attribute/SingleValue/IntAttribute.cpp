#include "IntAttribute.h"

namespace db
{
	bool IntAttribute::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("IntAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "int")
		{
			throw std::runtime_error("IntAttribute::readValueXML: Expected type 'int'.");
			return false;
		}

		if (!element->getDataAsInt(m_value))
		{
			throw std::runtime_error("IntAttribute::readValueXML: Failed to read integer value from XML element.");
			return false;
		}

		return true;
	}

	bool IntAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(getValue());
		return true;
	}
}
