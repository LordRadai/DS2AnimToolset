#include "AttributeArray.h"

namespace db
{
	bool AttributeArray::writeValueXML(int format)
	{
		if (writeStartArrayXML(format))
		{
			m_xmlElement->SetAttribute("elemType", m_type.c_str());

			for (size_t i = 0; i < m_attributes.size(); ++i)
			{
				if (!m_attributes[i]->writeXML(format))
					return false;
			}

			return true;
		}

		return false;
	}

	bool AttributeArray::writeStartArrayXML(int format) const
	{
		m_xmlElement->SetAttribute("size", size());
		return true;
	}
}
