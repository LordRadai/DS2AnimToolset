#include "CompositeAttribute.h"

namespace db
{
	bool CompositeAttribute::compare(Attribute* other)
	{
		CompositeAttribute* otherComposite = dynamic_cast<CompositeAttribute*>(other);

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::compare() failed - Other attribute is not a composite");

		if (otherComposite->getAttributeCount() != this->getAttributeCount())
			return false;

		for (size_t i = 0; i < getAttributeCount(); i++)
		{
			if (!m_attributes[i]->compare(otherComposite->getAttribute(i)))
				return false;
		}

		return true;
	}

	void CompositeAttribute::assign(Attribute* other)
	{
		CompositeAttribute* otherComposite = dynamic_cast<CompositeAttribute*>(other);

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::assign() failed - Other attribute is not a composite");

		//TODO: Figure out what it means to assign a composite attribute
	}

	int CompositeAttribute::getAttributeIndex(Attribute* attribute)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			if (m_attributes[i] == attribute)
				return static_cast<int>(i);
		}

		return -1;
	}

	void CompositeAttribute::deleteChildren()
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
			removeAt(i);
	}

	void CompositeAttribute::insertAttribute(int idx, Attribute* attribute)
	{
		if (attribute == nullptr)
			throw std::invalid_argument("CompositeAttribute::insertAttribute() failed - Attribute cannot be null");

		if (findAttribute(attribute->getName()))
			throw std::runtime_error("CompositeAttribute::insertAttribute() failed - Attribute with the same name already exists");

		m_attributes.insert(m_attributes.begin() + idx, attribute);
		attribute->setParentAttribute(this);
	}

	void CompositeAttribute::insertAttributeUnmanaged(int idx, Attribute* attribute)
	{
		m_attributes.insert(m_attributes.begin() + idx, attribute);
		attribute->setParentAttribute(this);
	}

	void CompositeAttribute::removeAttribute(Attribute* attribute)
	{
		if (attribute == nullptr)
			throw std::invalid_argument("CompositeAttribute::removeAttribute() failed - Attribute cannot be null");

		removeAttributeUnmanaged(attribute);
		delete attribute;
	}

	void CompositeAttribute::removeAttributeUnmanaged(Attribute* attribute)
	{
		int idx = getAttributeIndex(attribute);

		if (idx >= 0)
			m_attributes.erase(m_attributes.begin() + idx);

		attribute->setParentAttribute(nullptr);
	}

	void CompositeAttribute::removeAt(int idx)
	{
		if (idx >= m_attributes.size())
			removeAttribute(nullptr);

		removeAttribute(getAttribute(idx));
	}

	Attribute* CompositeAttribute::getAttribute(int idx) const
	{
		if (idx < m_attributes.size())
			return m_attributes[idx];

		throw std::out_of_range("CompositeAttribute::getAttrbute() - Index out of range");
	}

	Attribute* CompositeAttribute::findAttribute(std::string name) const
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			if (m_attributes[i]->getName() == name)
				return m_attributes[i];
		}

		return nullptr;
	}
}