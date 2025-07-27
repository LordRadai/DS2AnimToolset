#include "FloatAttribute.h"

namespace mcd
{
	FloatAttribute::FloatAttribute(db::Node* parent, std::string name, float value)
		: Attribute(parent, "FloatAttribute", name)
	{
		this->addFloatAttribute("Value", value);
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
}
