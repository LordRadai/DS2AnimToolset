#include "PointerAttribute.h"

namespace db
{
	bool PointerAttribute::writeValueXML(int format, SaverXML* saver)
	{
		m_xmlElement->SetText(m_value->toDatabasePath().c_str());
		return true;
	}
}