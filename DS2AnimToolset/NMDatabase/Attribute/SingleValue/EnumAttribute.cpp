#include "EnumAttribute.h"

namespace db
{
	bool EnumAttribute::writeValueXML(int format)
	{
		m_xmlElement->SetText(getValue());
		return true;
	}
}
