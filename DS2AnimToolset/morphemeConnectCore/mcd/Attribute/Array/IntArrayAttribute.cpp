#include "IntArrayAttribute.h"

namespace mcd
{
	IntArrayAttribute::IntArrayAttribute(db::Attribute* parent, std::string name)
		: Attribute(parent, "IntArrayAttribute", name),
		m_valueAttr(new db::IntArrayAttribute(this, "Value"))
	{
		this->addAttribute(m_valueAttr.get());
	}

	bool IntArrayAttribute::assignValue(Attribute* other)
	{
		IntArrayAttribute* otherAttr = dynamic_cast<IntArrayAttribute*>(other);

		if (otherAttr == nullptr)
			return false;

		this->m_valueAttr->assign(otherAttr->m_valueAttr.get());
		return true;
	}
	
	bool IntArrayAttribute::isValueEqualTo(Attribute* attr)
	{
		IntArrayAttribute* otherAttr = dynamic_cast<IntArrayAttribute*>(attr);

		if ((otherAttr == nullptr) || (this->m_valueAttr->size() != otherAttr->size()))
			return false;

		for (size_t i = 0; i < this->m_valueAttr->size(); i++)
		{
			if (this->m_valueAttr->getElement(i) != otherAttr->m_valueAttr->getElement(i))
				return false;
		}

		return true;
	}
}
