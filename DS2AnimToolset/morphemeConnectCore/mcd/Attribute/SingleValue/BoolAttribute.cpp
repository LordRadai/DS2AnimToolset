#include "BoolAttribute.h"

namespace mcd
{
	BoolAttribute::BoolAttribute(db::CompositeAttribute* parent, std::string name, bool value) :
		Attribute(parent, "BoolAttribute", name),
		m_valueAttr(new db::BoolAttribute(this, "Value", value))
	{
		setValue(value);
	}

	bool BoolAttribute::isValueEqualTo(Attribute* attr)
	{
		BoolAttribute* otherAttr = dynamic_cast<BoolAttribute*>(attr);
		
		if (otherAttr == nullptr)
			return false;

		return this->m_valueAttr->getValue() == otherAttr->m_valueAttr->getValue();
	}

	bool BoolAttribute::assignValue(Attribute* other)
	{
		BoolAttribute* otherBoolAttr = dynamic_cast<BoolAttribute*>(other);

		if (otherBoolAttr == nullptr)
			return false;

		this->m_valueAttr->assign(otherBoolAttr->m_valueAttr.get());
	}

	void BoolAttribute::setValue(bool value)
	{
		this->removeAttribute(m_valueAttr.get());

		if (value)
			this->addAttribute(m_valueAttr.get());

		m_valueAttr->setValue(value);
	}
}
