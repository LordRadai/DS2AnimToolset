#include "PointerAttribute.h"

namespace db
{
	bool PointerAttribute::readValueXML(int format, db::XMLElement* element)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("PointerAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "pointer")
		{
			throw std::runtime_error("PointerAttribute::readValueXML: Expected type 'pointer'.");
			return false;
		}

		if (!element->getDataAsString(m_value))
		{
			throw std::runtime_error("PointerAttribute::readValueXML: Failed to read pointer value from XML element.");
			return false;
		}

		return true;
	}

	bool PointerAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(m_value.c_str());
		return true;
	}
}