#include "BoolAttribute.h"

namespace db
{
	bool BoolAttribute::writeValueXML(int format)
	{
		m_xmlElement->SetText(static_cast<int>(getValue()));
		return true;
	}
}
