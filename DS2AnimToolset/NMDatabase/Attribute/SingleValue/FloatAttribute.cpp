#include "FloatAttribute.h"

namespace db
{
	bool FloatAttribute::writeValueXML(int format)
	{
		m_xmlElement->SetText(getValue());
		return true;
	}
}
