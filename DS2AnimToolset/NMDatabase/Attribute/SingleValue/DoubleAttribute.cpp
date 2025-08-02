#include "DoubleAttribute.h"

namespace db
{
	bool DoubleAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(getValue());
		return true;
	}
}
