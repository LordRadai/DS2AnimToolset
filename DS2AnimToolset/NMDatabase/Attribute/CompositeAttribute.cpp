#include "CompositeAttribute.h"

namespace db
{
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

		if (attribute->getParentAttribute())
			throw std::runtime_error("CompositeAttribute::insertAttribute() failed - Attribute is already in another container");

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