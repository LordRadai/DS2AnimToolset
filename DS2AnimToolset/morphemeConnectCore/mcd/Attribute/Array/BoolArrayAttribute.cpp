#include "BoolArrayAttribute.h"

namespace mcd
{
	BoolArrayAttribute::BoolArrayAttribute(db::Attribute* parent, std::string name)
		: Attribute(parent, "BoolArrayAttribute", name),
		m_valueAttr(new db::TypedAttributeArray<db::BoolAttribute>(this, "Value"))
	{
		this->addAttribute(this->m_valueAttr.get());
	}

	bool BoolArrayAttribute::assignValue(Attribute* other)
	{
		BoolArrayAttribute* otherAttr = dynamic_cast<BoolArrayAttribute*>(other);
		if (otherAttr == nullptr)
			return false;
		this->m_valueAttr->assign(otherAttr->m_valueAttr.get());
		return true;
	}

	bool BoolArrayAttribute::isValueEqualTo(Attribute* attr)
	{
		BoolArrayAttribute* otherAttr = dynamic_cast<BoolArrayAttribute*>(attr);
		
		if ((otherAttr == nullptr) || (this->m_valueAttr->size() != otherAttr->size()))
			return false;

		for (size_t i = 0; i < this->m_valueAttr->size(); i++)
		{
			if (this->getElement(i) != otherAttr->getElement(i))
				return false;
		}
		
		return true;
	}
}
