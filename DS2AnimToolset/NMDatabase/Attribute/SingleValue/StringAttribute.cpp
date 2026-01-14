#include "StringAttribute.h"

namespace db
{
	bool StringAttribute::readValueXML(int format, XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;
		m_xmlElement = element->getXmlElement();
		m_name = element->getName();
		m_type = element->getAttribute("type");

		if (!m_type.empty() && m_type != "string")
		{
			throw std::runtime_error("StringAttribute::readValueXML: Expected type 'string'.");
			return false;
		}

		const char* text = m_xmlElement->GetText();
		if (!text)
			return false;

		m_value = std::string(text);

		return true;
	}

	bool StringAttribute::writeValueXML(int format, SaverXML* saver)
	{
		const std::string value = getValue();

		m_xmlElement->SetText(getValue().c_str());
		return true;
	}
}