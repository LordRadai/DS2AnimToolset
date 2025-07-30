#include "BoolAttribute.h"

namespace mcd
{
	BoolAttribute::BoolAttribute(db::Attribute* parent, std::string name, bool value) : Attribute(parent, "BoolAttribute", name)
	{
		this->addBoolAttribute("Value", value);

		this->m_valueAttr.reset(this->getAttribute(0)->asBool());
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
}
