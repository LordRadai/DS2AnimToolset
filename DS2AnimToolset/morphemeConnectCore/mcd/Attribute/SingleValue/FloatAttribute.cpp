#include "FloatAttribute.h"

namespace mcd
{
	FloatAttribute::FloatAttribute(db::Node* parent, std::string name, float value)
		: Attribute(parent, "FloatAttribute", name)
	{
		this->addFloatAttribute("Value", value);
		this->m_valueAttr = this->getAttribute(0)->asFloat();
		setValue(value);
	}

	bool FloatAttribute::assignValue(Attribute* other)
	{
		FloatAttribute* otherAttr = dynamic_cast<FloatAttribute*>(other);

		if (otherAttr == nullptr)
			return false;

		this->m_valueAttr->assign(otherAttr->m_valueAttr);
	}
}
