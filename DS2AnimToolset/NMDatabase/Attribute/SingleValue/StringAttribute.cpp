#include "StringAttribute.h"

namespace db
{
	bool StringAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(getValue().c_str());
		return true;
	}
}