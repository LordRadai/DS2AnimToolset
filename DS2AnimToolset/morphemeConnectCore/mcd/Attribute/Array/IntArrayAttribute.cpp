#include "IntArrayAttribute.h"

namespace mcd
{
	IntArrayAttribute::IntArrayAttribute(db::Attribute* parent, std::string name)
		: Attribute(parent, "IntArrayAttribute", name),
		m_valueAttr(new db::TypedAttributeArray<db::IntAttribute>(this, "Value"))
	{
		this->addAttribute(m_valueAttr.get());
	}

	void IntArrayAttribute::removeElement(int index)
	{
		if (index < 0 || index >= static_cast<int>(m_valueAttr->size()))
			throw std::out_of_range("IntArrayAttribute::removeElement() - Index out of range");

		m_valueAttr->removeAt(index);

		if (m_valueAttr->empty())
			removeAttribute(m_valueAttr.get());
	}

	void IntArrayAttribute::addElement(int value)
	{
		removeAttribute(m_valueAttr.get());

		m_valueAttr->add(new db::IntAttribute(this, "elem", value));

		addAttribute(m_valueAttr.get());
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
			if (this->getElement(i) != otherAttr->getElement(i))
				return false;
		}

		return true;
	}
}
