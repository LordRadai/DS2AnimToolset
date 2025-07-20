#include "Condition.h"
#include "mcnSerializer/mcnSerializer.h"

namespace db
{
	tinyxml2::XMLElement* Condition::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);

		m_attributes.serialize(element);
		mcnSerializer::createStringElement(element, "Type", m_type);
		mcnSerializer::createIntElement(element, "ManifestVersion", m_manifestVersion);

		return element;
	}

	Attribute* Condition::getAttribute(int index) const
	{
		return dynamic_cast<Attribute*>(m_attributes.getNode(index));
	}

	Attribute* Condition::findAttribute(std::string name) const
	{
		for (size_t i = 0; i < m_attributes.getNumNodes(); ++i)
		{
			Attribute* attr = getAttribute(i);

			if (attr && attr->getName() == name)
				return attr;
		}

		return nullptr;
	}
}