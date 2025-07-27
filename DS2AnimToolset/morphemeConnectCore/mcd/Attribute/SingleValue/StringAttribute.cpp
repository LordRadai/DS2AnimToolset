#include "StringAttribute.h"

namespace mcd
{
	StringAttribute::StringAttribute(db::Node* parent, std::string name, const std::string& value)
		: Attribute(parent, "StringAttribute", name)
	{
		this->addStringAttribute("Value", value);
		this->m_valueAttr.reset(this->getAttribute(0)->asString());
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
}