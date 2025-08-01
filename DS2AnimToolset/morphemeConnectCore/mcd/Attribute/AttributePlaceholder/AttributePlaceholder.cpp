#include "AttributePlaceholder.h"

namespace mcd
{
	AttributePlaceholder::AttributePlaceholder(db::Attribute* parent, std::string name) :
		Node(parent, "AttributePlaceholder", name),
		m_attributes(this, "AttributeEntry") 
	{
		this->addAttribute(&m_attributes);
	}

	mcd::Attribute* AttributePlaceholder::getMcdAttribute() const
	{
		if (m_attributes.empty())
			return nullptr;

		return m_attributes.getNode(0);
	}

	void AttributePlaceholder::setMcdAttribute(mcd::Attribute* attribute)
	{
		if (!m_attributes.empty())
			m_attributes.removeAt(0);

		m_attributes.add(attribute);
		attribute->setParentAttribute(this);
	}
}
