#include "FloatAttribute.h"

namespace db
{
	bool FloatAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(getValue());
		return true;
	}
}
