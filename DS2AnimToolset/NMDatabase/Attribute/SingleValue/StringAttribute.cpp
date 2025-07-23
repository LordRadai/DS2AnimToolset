#include "StringAttribute.h"

namespace db
{
	bool StringAttribute::writeValueXML(int format)
	{
		m_xmlElement->SetText(getValue().c_str());
		return true;
	}
}