#include "QuaternionAttribute.h"

namespace db
{
	bool QuaternionAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->InsertNewChildElement("X")->SetText(m_value.x);
		m_xmlElement->InsertNewChildElement("Y")->SetText(m_value.y);
		m_xmlElement->InsertNewChildElement("Z")->SetText(m_value.z);
		m_xmlElement->InsertNewChildElement("W")->SetText(m_value.w);
		return true;
	}

	std::string QuaternionAttribute::getValueAsString() const
	{
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "(%f, %f, %f, %f)", m_value.x, m_value.y, m_value.z, m_value.w);

		return std::string(buffer);
	}
}
