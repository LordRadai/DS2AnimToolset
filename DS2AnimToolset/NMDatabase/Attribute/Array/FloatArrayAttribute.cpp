#include "FloatArrayAttribute.h"

namespace db
{
	bool FloatArrayAttribute::writeValueXML(int format)
	{
		if (writeStartArrayXML(format))
		{
			for (size_t i = 0; i < m_values.size(); ++i)
				m_xmlElement->InsertNewChildElement("fill")->SetText(m_values[i]);

			return true;
		}

		return false;
	}

	void FloatArrayAttribute::removeAt(int idx)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("FloatArrayAttribute::removeAt() - Index out of range");

		m_values.erase(m_values.begin() + idx);
	}

	bool FloatArrayAttribute::writeStartArrayXML(int format) const
	{
		m_xmlElement->SetAttribute("size", size());
	}

	void FloatArrayAttribute::insert(int idx, float value)
	{
		if (idx < 0 || idx > static_cast<int>(m_values.size()))
			throw std::out_of_range("FloatArrayAttribute::insert - Index out of range");

		m_values.insert(m_values.begin() + idx, value);
	}

	void FloatArrayAttribute::setElement(int idx, float value)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("FloatArrayAttribute::setElement - Index out of range");
		m_values[idx] = value;
	}

	float FloatArrayAttribute::getElement(int idx) const
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("FloatArrayAttribute::getElement - Index out of range");
		return m_values[idx];
	}
}