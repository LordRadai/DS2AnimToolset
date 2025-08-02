#include "EnumAttribute.h"

namespace db
{
	bool EnumAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(getValue().c_str());
		return true;
	}
}
