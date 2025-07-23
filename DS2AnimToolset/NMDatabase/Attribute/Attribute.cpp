#include "Attribute.h"
#include "Node/Node.h"
#include "Database/Database.h"

namespace db
{
	Database* Attribute::getDatabase() const
	{
		Attribute* parent = m_parent;

		while (parent)
		{
			if (parent == nullptr) return dynamic_cast<Database*>(parent);

			parent = parent->getParentAttribute();
		}
	}

	bool Attribute::writeStartElementXML(int format)
	{
		if (!m_parent)
			return false;

		m_xmlElement = m_parent->getXMLElement()->InsertNewChildElement(m_name.c_str());
		m_xmlElement->SetAttribute("type", m_type.c_str());

		if (m_label != "")
			m_xmlElement->SetAttribute("name", m_label.c_str());

		return true;
	}

	bool Attribute::writeXML(int format)
	{
		bool bWroteStartElem = writeStartElementXML(format);

		if (!bWroteStartElem)
			return false;

		return writeValueXML(format);
	}

	std::string Attribute::toDatabasePath() const
	{
		if (m_parent == nullptr)
			return m_name;

		return m_parent->toDatabasePath() + "." + m_name;
	}
}
