#include "BoolArrayAttribute.h"

namespace db
{
	bool BoolArrayAttribute::writeValueXML(int format)
	{
		if (writeStartArrayXML(format))
		{
			for (size_t i = 0; i < m_values.size(); ++i)
				m_xmlElement->InsertNewChildElement("fill")->SetText(m_values[i] ? "true" : "false");

			return true;
		}

		return false;
	}

	void BoolArrayAttribute::removeAt(int idx)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("BoolArrayAttribute::removeAt() - Index out of range");

		m_values.erase(m_values.begin() + idx);
	}

	bool BoolArrayAttribute::writeStartArrayXML(int format) const
	{
		m_xmlElement->SetAttribute("size", size());
	}

	void BoolArrayAttribute::insert(int idx, bool value)
	{
		if (idx < 0 || idx > static_cast<int>(m_values.size()))
			throw std::out_of_range("BoolArrayAttribute::insert - Index out of range");

		m_values.insert(m_values.begin() + idx, value);
	}

	void BoolArrayAttribute::setElement(int idx, bool value)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("BoolArrayAttribute::setElement - Index out of range");
		m_values[idx] = value;
	}

	bool BoolArrayAttribute::getElement(int idx) const
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("BoolArrayAttribute::getElement - Index out of range");
		return m_values[idx];
	}
}