#include "NodeContainer.h"

namespace db
{
	bool NodeContainer::writeValueXML(int format)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
			m_attributes[i]->writeXML();
	}
}