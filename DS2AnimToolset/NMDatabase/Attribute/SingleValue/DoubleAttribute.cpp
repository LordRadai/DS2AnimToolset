#include "DoubleAttribute.h"

namespace db
{
	bool DoubleAttribute::readValueXML(int format, db::XMLElement* element)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("DoubleAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");

		if (!m_type.empty() && m_type != "double")
		{
			throw std::runtime_error("DoubleAttribute::readValueXML: Expected type 'double'.");
			return false;
		}

		if (!element->getDataAsDouble(m_value))
		{
			throw std::runtime_error("DoubleAttribute::readValueXML: Failed to read double value from XML element.");
			return false;
		}

		return true;
	}

	bool DoubleAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(getValue());
		return true;
	}
}
