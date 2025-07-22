#include "Node.h"
#include "XMLWriter.h"

namespace db
{
	tinyxml2::XMLElement* Node::serialize(tinyxml2::XMLElement* parent)
	{
		return XMLWriter::createNodeElement(parent, m_identifier, m_label);
	}

	std::string Node::getQualifiedName() const
	{
		if (!m_parent)
			return m_label;

		return m_parent->getQualifiedName() + "." + m_label;
	}
}