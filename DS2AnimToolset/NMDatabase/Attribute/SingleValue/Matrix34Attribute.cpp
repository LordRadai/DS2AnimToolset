#include "Matrix34Attribute.h"

namespace db
{
	bool Matrix34Attribute::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("Matrix34Attribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "matrix34")
		{
			throw std::runtime_error("Matrix34Attribute::readValueXML: Expected type 'matrix34'.");
			return false;
		}
		
		if (!element->getDataAsMatrix34(m_value))
		{
			throw std::runtime_error("Matrix34Attribute::readValueXML: Failed to read matrix34 value from XML element.");
			return false;
		}

		return true;
	}

	bool Matrix34Attribute::writeValueXML(int format, SaverXML* saver)
	{
		for (uint32_t row = 0; row < 4; row++)
		{
			char rName[256];
			sprintf(rName, "R%d", row);

			tinyxml2::XMLElement* rowElement = m_xmlElement->InsertNewChildElement(rName);

			for (uint32_t col = 0; col < 3; col++)
			{
				char cName[256];
				sprintf(cName, "C%d", col);

				rowElement->InsertNewChildElement(cName)->SetText(getValue().r[row][col]);
			}
		}

		return true;
	}
}
