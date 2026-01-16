#include "IntAttribute.h"

namespace mcd
{
	IntAttribute::IntAttribute(db::CompositeAttribute* parent, std::string name, int value)
		: Attribute(parent, "IntAttribute", name),
		m_valueAttr(new db::IntAttribute(this, "Value", value))
	{
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

	void IntAttribute::setValue(int value)
	{
		this->removeAttribute(m_valueAttr.get());

		if (value != 0)
			this->addAttribute(m_valueAttr.get());

		m_valueAttr->setValue(value);
	}
}