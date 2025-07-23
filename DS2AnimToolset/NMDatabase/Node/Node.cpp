#include "Node.h"

namespace db
{
	bool Node::writeValueXML(int format)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
			m_attributes[i]->writeXML(format);
	}
}