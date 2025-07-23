#include "Node.h"

namespace db
{
	bool Node::writeValueXML(int format)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			if (!m_attributes[i]->writeXML(format))
				return false;
		}

		return true;
	}

	bool Node::writeStartElementXML(int format)
	{
		if (!m_parent)
			return false;

		m_xmlElement = m_parent->getXMLElement()->InsertNewChildElement(m_name.c_str());
		m_xmlElement->SetAttribute("name", m_nodeName.c_str());
		m_xmlElement->SetAttribute("type", "node");

		return true;
	}
}