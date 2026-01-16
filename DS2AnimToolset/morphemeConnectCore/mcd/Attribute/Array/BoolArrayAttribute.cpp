#include "BoolArrayAttribute.h"

namespace mcd
{
	BoolArrayAttribute::BoolArrayAttribute(db::CompositeAttribute* parent, std::string name)
		: Attribute(parent, "BoolArrayAttribute", name),
		m_valueAttr(new db::TypedAttributeArray<db::BoolAttribute>(this, "Value"))
	{}

	void BoolArrayAttribute::removeElement(int index)
	{
		if (index < 0 || index >= static_cast<int>(m_valueAttr->size()))
			throw std::out_of_range("BoolArrayAttribute::removeElement() - Index out of range");

		m_valueAttr->removeAt(index);

		if (m_valueAttr->empty())
			removeAttribute(m_valueAttr.get());
	}

	void BoolArrayAttribute::addElement(bool value)
	{
		if (m_valueAttr->empty())
			addAttribute(m_valueAttr.get());

		m_valueAttr->add(new db::BoolAttribute(this, "elem", value));
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
