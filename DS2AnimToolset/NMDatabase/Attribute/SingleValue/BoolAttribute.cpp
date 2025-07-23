#include "BoolAttribute.h"

namespace db
{
	bool BoolAttribute::writeValueXML(int format)
	{
		m_xmlElement->SetText(getValue() ? "true" : "false");
		return true;
	}
}
