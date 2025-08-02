#include "AttributeArray.h"

namespace db
{
	bool AttributeArray::writeValueXML(int format, SaverXML* saver)
	{
		if (writeStartArrayXML(format, saver))
		{
			m_xmlElement->SetAttribute("elemType", getElemType().c_str());

			for (size_t i = 0; i < m_attributes.size(); ++i)
			{
				if (!m_attributes[i]->writeXML(format, saver))
					return false;
			}

			return true;
		}

		return false;
	}

	bool AttributeArray::writeStartArrayXML(int format, SaverXML* saver) const
	{
		m_xmlElement->SetAttribute("size", size());
		return true;
	}
}
