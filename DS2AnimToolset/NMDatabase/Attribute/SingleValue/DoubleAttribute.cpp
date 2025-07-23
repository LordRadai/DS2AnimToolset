#include "DoubleAttribute.h"

namespace db
{
	bool DoubleAttribute::writeValueXML(int format)
	{
		m_xmlElement->SetText(getValue());
		return true;
	}
}
