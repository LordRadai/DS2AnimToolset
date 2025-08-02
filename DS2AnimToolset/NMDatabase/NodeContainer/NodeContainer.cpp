#include "NodeContainer.h"

namespace db
{
	bool NodeContainer::writeValueXML(int format, SaverXML* saver)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			if (!m_attributes[i]->writeXML(format, saver))
				return false;
		}

		return true;
	}
}