#include "FloatArrayAttribute.h"

namespace mcd
{
	FloatArrayAttribute::FloatArrayAttribute(db::CompositeAttribute* parent, std::string name)
		: Attribute(parent, "FloatArrayAttribute", name),
		m_valueAttr(new db::TypedAttributeArray<db::FloatAttribute>(this, "Value", "float"))
	{
	}

	void FloatArrayAttribute::removeElement(int index)
	{
		if (index < 0 || index >= static_cast<int>(m_valueAttr->size()))
			throw std::out_of_range("FloatArrayAttribute::removeElement() - Index out of range");

		m_valueAttr->removeAt(index);

		if (m_valueAttr->empty())
			removeAttribute(m_valueAttr.get());
	}

	void FloatArrayAttribute::addElement(float value)
	{
		if (m_valueAttr->empty())
			addAttribute(m_valueAttr.get());

		db::FloatAttribute* attr = m_valueAttr->add();
		attr->setValue(value);
	}

	bool FloatArrayAttribute::assignValue(Attribute* other)
	{
		FloatArrayAttribute* otherAttr = dynamic_cast<FloatArrayAttribute*>(other);

		if (otherAttr == nullptr)
			return false;

		this->m_valueAttr->assign(otherAttr->m_valueAttr.get());
		return true;
	}

	bool FloatArrayAttribute::isValueEqualTo(Attribute* attr)
	{
		FloatArrayAttribute* otherAttr = dynamic_cast<FloatArrayAttribute*>(attr);
		
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
