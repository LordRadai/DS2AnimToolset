#include "RefArrayAttribute.h"

namespace mcd
{
	RefArrayAttribute::RefArrayAttribute(db::CompositeAttribute* parent, std::string name)
		: Attribute(parent, "RefArrayAttribute", name),
		m_valueAttr(std::make_unique<db::PointerArray<AttributePinNodeBase>>(this, "Value"))
	{
		this->addAttribute(m_valueAttr.get());
	}

	void RefArrayAttribute::removeElement(int index)
	{
		if (index < 0 || index >= static_cast<int>(m_valueAttr->size()))
			throw std::out_of_range("RefArrayAttribute::removeElement() - Index out of range");

		m_valueAttr->removeAt(index);

		if (m_valueAttr->empty())
			removeAttribute(m_valueAttr.get());
	}

	void RefArrayAttribute::addElement(AttributePinNodeBase* value)
	{
		if (value == nullptr)
			throw std::invalid_argument("RefArrayAttribute::addElement() - Value cannot be null");

		m_valueAttr->add(value);
	}

	void RefArrayAttribute::setElement(int index, AttributePinNodeBase* value)
	{
		if (index < 0 || index >= static_cast<int>(m_valueAttr->size()))
			throw std::out_of_range("RefArrayAttribute::setElement() - Index out of range");

		if (value == nullptr)
			throw std::invalid_argument("RefArrayAttribute::setElement() - Value cannot be null");

		m_valueAttr->getAttribute(index)->assign(value);
	}

	AttributePinNodeBase* RefArrayAttribute::getElement(int index)
	{
		if (index < 0 || index >= static_cast<int>(m_valueAttr->size()))
			throw std::out_of_range("RefArrayAttribute::getElement() - Index out of range");

		return dynamic_cast<AttributePinNodeBase*>(m_valueAttr->getAttribute(index));
	}

	bool RefArrayAttribute::assignValue(Attribute* other)
	{
		RefArrayAttribute* otherAttr = dynamic_cast<RefArrayAttribute*>(other);

		if (otherAttr == nullptr)
			return false;

		this->m_valueAttr->assign(otherAttr->m_valueAttr.get());

		return true;
	}

	bool RefArrayAttribute::isValueEqualTo(Attribute* attr)
	{
		RefArrayAttribute* otherAttr = dynamic_cast<RefArrayAttribute*>(attr);

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
