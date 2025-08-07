#include "NodeContainer.h"

namespace db
{
	bool NodeContainer::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		// TODO Implement reading XML for NodeContainer
		return false;
	}

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