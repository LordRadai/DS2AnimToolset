#include "FloatAttribute.h"

namespace mcd
{
	FloatAttribute::FloatAttribute(db::Attribute* parent, std::string name, float value)
		: Attribute(parent, "FloatAttribute", name),
		m_valueAttr(new db::FloatAttribute(this, "Value", value))
	{
		this->m_valueAttr.reset(this->getAttribute(0)->asFloat());
		setValue(value);
	}

	bool FloatAttribute::assignValue(Attribute* other)
	{
		FloatAttribute* otherAttr = dynamic_cast<FloatAttribute*>(other);

		if (otherAttr == nullptr)
			return false;

		this->m_valueAttr->assign(otherAttr->m_valueAttr.get());
	}

	bool FloatAttribute::isValueEqualTo(Attribute* attr)
	{
		FloatAttribute* otherAttr = dynamic_cast<FloatAttribute*>(attr);

		if (otherAttr == nullptr)
			return false;

		return ((this->m_valueAttr->getValue() != NAN) &&
			(otherAttr->m_valueAttr->getValue() != NAN) &&
			(this->m_valueAttr->getValue() == otherAttr->m_valueAttr->getValue()));
	}

	void FloatAttribute::setValue(float value)
	{
		this->removeAttribute(m_valueAttr.get());

		if (value != 0.0f)
			this->addAttribute(m_valueAttr.get());

		m_valueAttr->setValue(value);
	}
}
