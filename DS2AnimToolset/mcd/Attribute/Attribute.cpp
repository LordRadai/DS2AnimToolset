#include "Attribute.h"

namespace mcd
{
	void AttributePlaceholder::setAttribute(Attribute* attribute)
	{
		if (m_attribute)
			delete m_attribute;

		m_attribute = attribute;
	}

	void AnimationSetAttribute::setAttribute(Attribute* attribute)
	{
		if (getNumAttributePlaceholders() == 0)
		{
			addAttributePlaceholder(new AttributePlaceholder(this, m_targetAnimSet, attribute));
			return;
		}

		if (getNumAttributePlaceholders() > 1)
		{
			throw(std::runtime_error("Invalid number of AttributePlaceholder objects in AnimationSetAttribute\n"));
			return;
		}

		AttributePlaceholder* placeholder = getAttributePlaceholder(0);
		placeholder->setAttribute(attribute);
	}
}
