#include "BoolAttribute.h"

namespace db
{
	bool BoolAttribute::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("BoolAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");

		if (!m_type.empty() && m_type != "bool")
		{
			throw std::runtime_error("BoolAttribute::readValueXML: Expected type 'bool'.");
			return false;
		}

		if (!element->getDataAsBool(m_value))
		{
			throw std::runtime_error("BoolAttribute::readValueXML: Failed to read boolean value from XML element.");
			return false;
		}

		return true;
	}

	bool BoolAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(static_cast<int>(getValue()));
		return true;
	}
}
