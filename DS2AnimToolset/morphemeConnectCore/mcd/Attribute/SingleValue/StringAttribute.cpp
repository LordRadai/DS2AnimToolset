#include "StringAttribute.h"

namespace mcd
{
	StringAttribute::StringAttribute(db::Attribute* parent, std::string name, const std::string& value)
		: Attribute(parent, "StringAttribute", name),
		m_valueAttr(new db::StringAttribute(this, "Value", value))
	{
		setValue(value);
	}

	bool StringAttribute::assignValue(Attribute* other)
	{
		StringAttribute* otherAttr = dynamic_cast<StringAttribute*>(other);

		if (otherAttr == nullptr)
			return false;

		this->m_valueAttr->assign(otherAttr->m_valueAttr.get());
		return true;
	}

	bool StringAttribute::isValueEqualTo(Attribute* attr)
	{
		StringAttribute* otherAttr = dynamic_cast<StringAttribute*>(attr);

		if (otherAttr == nullptr)
			return false;

		return this->m_valueAttr->getValue() == otherAttr->m_valueAttr->getValue();
	}

	void StringAttribute::setValue(const std::string& value)
	{
		this->removeAttribute(m_valueAttr.get());

		if (!value.empty())
			this->addAttribute(m_valueAttr.get());

		m_valueAttr->setValue(value);
	}
}