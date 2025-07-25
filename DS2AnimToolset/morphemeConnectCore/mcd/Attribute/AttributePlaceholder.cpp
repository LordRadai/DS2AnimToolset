#include "AttributePlaceholder.h"

namespace mcd
{
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
	}
}
