#include "QuaternionAttribute.h"

namespace db
{
	bool QuaternionAttribute::writeValueXML(int format)
	{
		m_xmlElement->InsertNewChildElement("X")->SetText(m_value.x);
		m_xmlElement->InsertNewChildElement("Y")->SetText(m_value.y);
		m_xmlElement->InsertNewChildElement("Z")->SetText(m_value.z);
		m_xmlElement->InsertNewChildElement("W")->SetText(m_value.w);
		return true;
	}
}
