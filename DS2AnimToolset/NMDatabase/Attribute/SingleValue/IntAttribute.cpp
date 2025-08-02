#include "IntAttribute.h"

namespace db
{
	bool IntAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(getValue());
		return true;
	}
}
