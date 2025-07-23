#include "PointerAttribute.h"

namespace db
{
	bool PointerAttribute::writeValueXML(int format)
	{
		m_xmlElement->SetText(m_value.c_str());
		return true;
	}
}