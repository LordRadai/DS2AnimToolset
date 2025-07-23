#include "Matrix34ArrayAttribute.h"

namespace db
{
	bool Matrix34ArrayAttribute::writeValueXML(int format)
	{
		if (writeStartArrayXML(format))
		{
			for (size_t i = 0; i < m_values.size(); ++i)
			{
				for (uint32_t row = 0; row < 4; row++)
				{
					for (uint32_t col = 0; col < 3; col++)
					{
						char name[256];
						sprintf(name, "fill_%d%d", row, col);

						//m_xmlElement->InsertNewChildElement(name)->SetText(m_values[i].r[row][col]);
					}
				}
			}

			return true;
		}

		return false;
	}

	void Matrix34ArrayAttribute::removeAt(int idx)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("Matrix34ArrayAttribute::removeAt() - Index out of range");

		m_values.erase(m_values.begin() + idx);
	}

	bool Matrix34ArrayAttribute::writeStartArrayXML(int format) const
	{
		m_xmlElement->SetAttribute("size", size());
	}

	void Matrix34ArrayAttribute::insert(int idx, NMP::Matrix34 value)
	{
		if (idx < 0 || idx > static_cast<int>(m_values.size()))
			throw std::out_of_range("Matrix34ArrayAttribute::insert - Index out of range");

		m_values.insert(m_values.begin() + idx, value);
	}

	void Matrix34ArrayAttribute::setElement(int idx, NMP::Matrix34 value)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("Matrix34ArrayAttribute::setElement - Index out of range");
		m_values[idx] = value;
	}

	NMP::Matrix34 Matrix34ArrayAttribute::getElement(int idx) const
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("Matrix34ArrayAttribute::getElement - Index out of range");
		return m_values[idx];
	}
}