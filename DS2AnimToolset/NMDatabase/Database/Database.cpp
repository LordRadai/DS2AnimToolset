#include "Database.h"
#include "Saver/SaverXML.h"

namespace db
{
	Node* Database::find(std::string name) const
	{
		for (size_t i = 0; i < getNumNodes(); i++)
		{
			Node* node = getNode(i);

			if (node->getName() == name)
				return node;
		}

		return nullptr;
	}

	bool Database::writeStartElementXML(int format, SaverXML* saver)
	{
		m_xmlElement = saver->getXMLDocument()->NewElement(m_name.c_str());

		if (m_nodeName != "")
			m_xmlElement->SetAttribute("name", m_nodeName.c_str());

		m_xmlElement->SetAttribute("type", "node");
		m_xmlElement->SetAttribute("namespace", m_namespace.c_str());

		return true;
	}
}