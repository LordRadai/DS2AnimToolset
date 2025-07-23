#include "IntAttribute.h"

namespace db
{
	bool IntAttribute::writeValueXML(int format)
	{
		m_xmlElement->SetText(getValue());
		return true;
	}
}
