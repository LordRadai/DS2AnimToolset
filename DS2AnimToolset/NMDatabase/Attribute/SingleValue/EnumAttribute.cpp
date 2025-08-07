#include "EnumAttribute.h"

namespace db
{
	bool EnumAttribute::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("EnumAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "enum")
		{
			throw std::runtime_error("EnumAttribute::readValueXML: Expected type 'enum'.");
			return false;
		}

		if (!element->getDataAsString(m_value))
		{
			throw std::runtime_error("EnumAttribute::readValueXML: Failed to read enum value from XML element.");
			return false;
		}

		return true;
	}

	bool EnumAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(getValue().c_str());
		return true;
	}
}
