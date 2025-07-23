#include "Attribute.h"
#include "Node/Node.h"

namespace db
{
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
