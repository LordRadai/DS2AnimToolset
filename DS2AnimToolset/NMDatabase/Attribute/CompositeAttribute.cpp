#include "CompositeAttribute.h"

namespace db
{
	bool CompositeAttribute::compare(Attribute* other)
	{
		CompositeAttribute* otherComposite = other->asComposite();

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::compare() failed - Other attribute is not a composite");
		
		if (this->getAttributeCount() != otherComposite->getAttributeCount())
			return false;

		for (size_t i = 0; i < getAttributeCount(); i++)
		{
			Attribute* thisAttribute = getAttribute(i);
			Attribute* otherAttribute = otherComposite->getAttribute(i);

			if (!thisAttribute->compare(otherAttribute))
				return false;
		}

		return true;
	}

	void CompositeAttribute::assign(Attribute* other)
	{
		CompositeAttribute* otherComposite = dynamic_cast<CompositeAttribute*>(other);

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::assign() failed - Other attribute is not a composite");

		if (getAttributeCount() != otherComposite->getAttributeCount())
			throw std::runtime_error("CompositeAttribute::assign() failed - Size mismatch");

		for (size_t i = 0; i < getAttributeCount(); i++)
		{
			Attribute* thisAttribute = getAttribute(i);
			Attribute* otherAttribute = nullptr;

			if (i < otherComposite->getAttributeCount())
				otherAttribute = otherComposite->getAttribute(i);

			thisAttribute->assign(otherAttribute);
		}
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
			return;

		removeAttributeUnmanaged(attribute);
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