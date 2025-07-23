#include "QuaternionArrayAttribute.h"

namespace db
{
	bool QuaternionArrayAttribute::compare(Attribute* other)
	{
		QuaternionArrayAttribute* otherTyped = dynamic_cast<QuaternionArrayAttribute*>(other);

		if (otherTyped == nullptr)
			throw std::runtime_error("QuaternionArrayAttribute::compare() failed - Other attribute is not a bool array");

		if (otherTyped->size() != this->size())
			return false;

		for (size_t i = 0; i < size(); i++)
		{
			if (m_values[i] != otherTyped->getElement(i))
				return false;
		}

		return true;
	}

	void QuaternionArrayAttribute::assign(Attribute* other)
	{
		QuaternionArrayAttribute* otherComposite = dynamic_cast<QuaternionArrayAttribute*>(other);

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::assign() failed - Other attribute is not a composite");

		m_values.clear();

		m_values.reserve(otherComposite->size());
		for (size_t i = 0; i < otherComposite->size(); ++i)
			m_values.push_back(otherComposite->getElement(i));
	}

	bool QuaternionArrayAttribute::writeValueXML(int format)
	{
		if (writeStartArrayXML(format))
		{
			for (size_t i = 0; i < m_values.size(); ++i)
			{
				//m_xmlElement->InsertNewChildElement("e")->SetText(m_values[i]);
			}

			return true;
		}

		return false;
	}

	void QuaternionArrayAttribute::removeAt(int idx)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("QuaternionArrayAttribute::removeAt() - Index out of range");

		m_values.erase(m_values.begin() + idx);
	}

	bool QuaternionArrayAttribute::writeStartArrayXML(int format) const
	{
		m_xmlElement->SetAttribute("size", size());
		return true;
	}

	void QuaternionArrayAttribute::insert(int idx, NMP::Quat value)
	{
		if (idx < 0 || idx > static_cast<int>(m_values.size()))
			throw std::out_of_range("QuaternionArrayAttribute::insert - Index out of range");

		m_values.insert(m_values.begin() + idx, value);
	}

	void QuaternionArrayAttribute::setElement(int idx, NMP::Quat value)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("QuaternionArrayAttribute::setElement - Index out of range");
		m_values[idx] = value;
	}

	NMP::Quat QuaternionArrayAttribute::getElement(int idx) const
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("QuaternionArrayAttribute::getElement - Index out of range");
		return m_values[idx];
	}
}