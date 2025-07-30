#include "IntAttribute.h"

namespace mcd
{
	IntAttribute::IntAttribute(db::Attribute* parent, std::string name, int value)
		: Attribute(parent, "IntAttribute", name)
	{
		this->addIntAttribute("Value", value);
		this->m_valueAttr.reset(this->getAttribute(0)->asInt());
		setValue(value);
	}

	bool IntAttribute::assignValue(Attribute* other)
	{
		IntAttribute* otherAttr = dynamic_cast<IntAttribute*>(other);

		if (otherAttr == nullptr)
			return false;

		this->m_valueAttr->assign(otherAttr->m_valueAttr.get());
		return true;
	}

	bool IntAttribute::isValueEqualTo(Attribute* attr)
	{
		IntAttribute* otherAttr = dynamic_cast<IntAttribute*>(attr);

		if (otherAttr == nullptr)
			return false;

		return this->m_valueAttr->getValue() == otherAttr->m_valueAttr->getValue();
	}
}