#include "Node.h"
#include "mcnSerializer/mcnSerializer.h"

namespace db
{
	tinyxml2::XMLElement* Node::serialize(tinyxml2::XMLElement* parent)
	{
		return mcnSerializer::createNodeElement(parent, m_identifier, m_label);
	}

	std::string Node::getQualifiedName() const
	{
		if (!m_parent)
			return m_label;

		return m_parent->getQualifiedName() + "." + m_label;
	}
}