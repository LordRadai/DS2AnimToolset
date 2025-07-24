#include "BoolAttribute.h"

namespace mcd
{
	BoolAttribute::BoolAttribute(db::Node* parent, std::string name, bool value) : Attribute(parent, "BoolAttribute", name)
	{
		this->addBoolAttribute("Value", value);

		this->m_valueAttr = this->getAttribute(0)->asBool();
		setValue(value);
	}

	bool BoolAttribute::assignValue(Attribute* other)
	{
		BoolAttribute* otherBoolAttr = dynamic_cast<BoolAttribute*>(other);

		if (otherBoolAttr == nullptr)
			return false;

		this->m_valueAttr->assign(otherBoolAttr->m_valueAttr);
	}
}
