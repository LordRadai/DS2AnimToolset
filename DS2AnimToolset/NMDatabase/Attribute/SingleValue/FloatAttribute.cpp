#include "FloatAttribute.h"

namespace db
{
	bool FloatAttribute::readValueXML(int format, db::XMLElement* element)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("FloatAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "float")
		{
			throw std::runtime_error("FloatAttribute::readValueXML: Expected type 'float'.");
			return false;
		}

		if (!element->getDataAsFloat(m_value))
		{
			throw std::runtime_error("FloatAttribute::readValueXML: Failed to read float value from XML element.");
			return false;
		}

		return true;
	}

	bool FloatAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(getValue());
		return true;
	}
}
