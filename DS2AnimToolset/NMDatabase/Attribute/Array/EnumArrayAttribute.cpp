#include "EnumArrayAttribute.h"

namespace db
{
	bool EnumArrayAttribute::compare(Attribute* other)
	{
		EnumArrayAttribute* otherTyped = other->asEnumArray();

		if (otherTyped == nullptr)
			throw std::runtime_error("EnumArrayAttribute::compare() failed - Other attribute is not a bool array");

		if (otherTyped->size() != this->size())
			return false;

		for (size_t i = 0; i < size(); i++)
		{
			if (m_values[i] != otherTyped->getElement(i))
				return false;
		}

		return true;
	}

	void EnumArrayAttribute::assign(Attribute* other)
	{
		EnumArrayAttribute* otherComposite = other->asEnumArray();

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::assign() failed - Other attribute is not a composite");

		m_values.clear();

		m_values.reserve(otherComposite->size());
		for (size_t i = 0; i < otherComposite->size(); ++i)
			m_values.push_back(otherComposite->getElement(i));
	}

	bool EnumArrayAttribute::writeValueXML(int format, SaverXML* saver)
	{
		if (writeStartArrayXML(format, saver))
		{
			for (size_t i = 0; i < m_values.size(); ++i)
				m_xmlElement->InsertNewChildElement("fill")->SetText(m_values[i].c_str());

			return true;
		}

		return false;
	}

	void EnumArrayAttribute::removeAt(int idx)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("EnumArrayAttribute::removeAt() - Index out of range");

		m_values.erase(m_values.begin() + idx);
	}

	bool EnumArrayAttribute::writeStartArrayXML(int format, SaverXML* saver) const
	{
		m_xmlElement->SetAttribute("size", size());
		return true;
	}

	void EnumArrayAttribute::insert(int idx, std::string value)
	{
		if (idx < 0 || idx > static_cast<int>(m_values.size()))
			throw std::out_of_range("EnumArrayAttribute::insert - Index out of range");

		m_values.insert(m_values.begin() + idx, value);
	}

	void EnumArrayAttribute::setElement(int idx, std::string value)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("EnumArrayAttribute::setElement - Index out of range");
		m_values[idx] = value;
	}

	std::string EnumArrayAttribute::getElement(int idx) const
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("EnumArrayAttribute::getElement - Index out of range");
		return m_values[idx];
	}
}