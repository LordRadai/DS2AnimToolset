#include "Vector3ArrayAttribute.h"

namespace db
{
	bool Vector3ArrayAttribute::writeValueXML(int format)
	{
		if (writeStartArrayXML(format))
		{
			//for (size_t i = 0; i < m_values.size(); ++i)
				//m_xmlElement->InsertNewChildElement("e")->SetText(m_values[i] ? "true" : "false");

			return true;
		}

		return false;
	}

	void Vector3ArrayAttribute::removeAt(int idx)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("Vector3ArrayAttribute::removeAt() - Index out of range");

		m_values.erase(m_values.begin() + idx);
	}

	bool Vector3ArrayAttribute::writeStartArrayXML(int format) const
	{
		m_xmlElement->SetAttribute("size", size());
		return true;
	}

	void Vector3ArrayAttribute::insert(int idx, NMP::Vector3 value)
	{
		if (idx < 0 || idx > static_cast<int>(m_values.size()))
			throw std::out_of_range("Vector3ArrayAttribute::insert - Index out of range");

		m_values.insert(m_values.begin() + idx, value);
	}

	void Vector3ArrayAttribute::setElement(int idx, NMP::Vector3 value)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("Vector3ArrayAttribute::setElement - Index out of range");
		m_values[idx] = value;
	}

	NMP::Vector3 Vector3ArrayAttribute::getElement(int idx) const
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("Vector3ArrayAttribute::getElement - Index out of range");
		return m_values[idx];
	}
}