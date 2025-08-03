#include "StringAttribute.h"

namespace db
{
	bool StringAttribute::readValueXML(int format, tinyxml2::XMLElement* element)
	{
		if (!element)
			return false;
		m_xmlElement = element;
		m_name = element->Name();
		m_type = element->Attribute("type");

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
		m_xmlElement->SetText(getValue().c_str());
		return true;
	}
}