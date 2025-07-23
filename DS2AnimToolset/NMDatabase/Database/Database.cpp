#include "Database.h"

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

	bool Database::writeStartElementXML(int format)
	{
		if (m_xmlElement == nullptr)
			return false;

		m_xmlElement->SetAttribute("name", "Database");
		m_xmlElement->SetAttribute("type", "node");
		return true;
	}

	void Database::exportXML(const char* filepath)
	{
		tinyxml2::XMLDocument doc;

		m_xmlElement = doc.NewElement("Database");
		doc.InsertFirstChild(m_xmlElement);

		if (!writeXML())
			throw std::runtime_error("Failed to write XML for Database.");

		doc.SaveFile(filepath);
	}
}