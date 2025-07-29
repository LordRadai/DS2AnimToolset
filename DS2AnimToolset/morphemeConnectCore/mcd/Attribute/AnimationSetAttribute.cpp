#include "AnimationSetAttribute.h"

namespace mcd
{
	AnimationSetAttribute::AnimationSetAttribute(db::Node* parent, const std::string& name, const std::string& animSetName, mcd::Attribute* attribute)
		: Attribute(parent, name, "AnimationSetAttribute"), m_attributePlaceholders(std::make_unique<db::TypedNodeContainer<mcd::AttributePlaceholder>>(this, "AttributePlaceholders"))
	{
		addAttribute(m_attributePlaceholders.get());

		mcd::AttributePlaceholder* placeholder = new AttributePlaceholder(this, animSetName);
		placeholder->setMcdAttribute(attribute);

		m_attributePlaceholders->add(placeholder);
	}

	void AnimationSetAttribute::setAttribute(const std::string& animSetName, mcd::Attribute* attribute)
	{
		if (m_attributePlaceholders->size())
			m_attributePlaceholders->clearArray();

		mcd::AttributePlaceholder* placeholder = new AttributePlaceholder(this, animSetName);
		placeholder->setMcdAttribute(attribute);

		m_attributePlaceholders->add(placeholder);
	}
}
