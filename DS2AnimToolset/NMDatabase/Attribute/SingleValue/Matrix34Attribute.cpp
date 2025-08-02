#include "Matrix34Attribute.h"

namespace db
{
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
