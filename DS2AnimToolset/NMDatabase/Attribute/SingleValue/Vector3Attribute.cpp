#include "Vector3Attribute.h"

namespace db
{
	std::string Vector3Attribute::getValueAsString() const
	{
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "(%f, %f, %f)", m_value.x, m_value.y, m_value.z);

		return std::string(buffer);
	}

	bool Vector3Attribute::writeValueXML(int format)
	{
		m_xmlElement->InsertNewChildElement("X")->SetText(m_value.x);
		m_xmlElement->InsertNewChildElement("Y")->SetText(m_value.y);
		m_xmlElement->InsertNewChildElement("Z")->SetText(m_value.z);
		return true;
	}
}