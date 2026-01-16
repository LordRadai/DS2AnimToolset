#include "AnimationSetAttribute.h"

namespace mcd
{
	AnimationSetAttribute::AnimationSetAttribute(db::CompositeAttribute* parent, const std::string& attribName, const std::string& animSetName, mcd::Attribute* attribute)
		: Attribute(parent, "AnimationSetAttribute", attribName), m_attributePlaceholders(std::make_unique<db::TypedNodeContainer<mcd::AttributePlaceholder>>(this, "AttributePlaceholders"))
	{
		addAttribute(m_attributePlaceholders.get());

		setAttribute(animSetName, attribute);
	}

	void AnimationSetAttribute::setAttribute(const std::string& animSetName, mcd::Attribute* attribute)
	{
		if (m_attributePlaceholders->size())
			m_attributePlaceholders->clearArray();

		mcd::AttributePlaceholder* placeholder = new AttributePlaceholder(this, animSetName);
		placeholder->setMcdAttribute(attribute);

		m_attributePlaceholders->add(placeholder);
	}

	mcd::Attribute* AnimationSetAttribute::getMcdAttribute(const std::string& animSetName) const
	{
		mcd::AttributePlaceholder* placeholder = m_attributePlaceholders->find(animSetName);

		if (placeholder)
			return placeholder->getMcdAttribute();

		return nullptr;
	}
}
