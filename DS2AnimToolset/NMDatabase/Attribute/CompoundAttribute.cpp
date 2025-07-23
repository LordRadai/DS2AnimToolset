#include "CompoundAttribute.h"

namespace db
{
	bool CompoundAttribute::writeValueXML(int format)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			if (!m_attributes[i]->writeXML(format))
				return false;
		}

		return true;
	}
}
