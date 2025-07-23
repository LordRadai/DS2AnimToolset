#include "StringArrayAttribute.h"

namespace db
{
	bool StringArrayAttribute::writeValueXML(int format)
	{
		if (writeStartArrayXML(format))
		{
			for (size_t i = 0; i < m_values.size(); ++i)
				m_xmlElement->InsertNewChildElement("e")->SetText(m_values[i].c_str());

			return true;
		}

		return false;
	}

	void StringArrayAttribute::removeAt(int idx)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("StringArrayAttribute::removeAt() - Index out of range");

		m_values.erase(m_values.begin() + idx);
	}

	bool StringArrayAttribute::writeStartArrayXML(int format) const
	{
		m_xmlElement->SetAttribute("size", size());
	}

	void StringArrayAttribute::insert(int idx, std::string value)
	{
		if (idx < 0 || idx > static_cast<int>(m_values.size()))
			throw std::out_of_range("StringArrayAttribute::insert - Index out of range");

		m_values.insert(m_values.begin() + idx, value);
	}

	void StringArrayAttribute::setElement(int idx, std::string value)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("StringArrayAttribute::setElement - Index out of range");
		m_values[idx] = value;
	}

	std::string StringArrayAttribute::getElement(int idx) const
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("StringArrayAttribute::getElement - Index out of range");
		return m_values[idx];
	}
}