#include "Matrix34ArrayAttribute.h"

namespace db
{
	bool Matrix34ArrayAttribute::compare(Attribute* other)
	{
		Matrix34ArrayAttribute* otherTyped = other->asMatrix34Array();

		if (otherTyped == nullptr)
			throw std::runtime_error("Matrix34ArrayAttribute::compare() failed - Other attribute is not a bool array");

		if (otherTyped->size() != this->size())
			return false;

		for (size_t i = 0; i < size(); i++)
		{
			if (m_values[i] != otherTyped->getElement(i))
				return false;
		}

		return true;
	}

	void Matrix34ArrayAttribute::assign(Attribute* other)
	{
		Matrix34ArrayAttribute* otherComposite = other->asMatrix34Array();

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::assign() failed - Other attribute is not a composite");

		m_values.clear();

		m_values.reserve(otherComposite->size());
		for (size_t i = 0; i < otherComposite->size(); ++i)
			m_values.push_back(otherComposite->getElement(i));
	}

	bool Matrix34ArrayAttribute::writeValueXML(int format, SaverXML* saver)
	{
		if (writeStartArrayXML(format, saver))
		{
			for (size_t i = 0; i < m_values.size(); ++i)
			{
				for (uint32_t row = 0; row < 4; row++)
				{
					for (uint32_t col = 0; col < 3; col++)
					{
						char name[256];
						sprintf(name, "elem_%d%d", row, col);

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

	bool Matrix34ArrayAttribute::readValueXML(int format, db::XMLElement* element)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("Matrix34ArrayAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "matrix34Array")
		{
			throw std::runtime_error("Matrix34ArrayAttribute::readValueXML: Expected type 'matrix34Array'.");
			return false;
		}
		
		if (!element->getDataAsMatrix34Array(m_values))
		{
			throw std::runtime_error("Matrix34ArrayAttribute::readValueXML: Failed to read matrix34 array value from XML element.");
			return false;
		}

		return true;
	}

	bool Matrix34ArrayAttribute::writeStartArrayXML(int format, SaverXML* saver) const
	{
		if (size() == 0)
			return false;

		m_xmlElement->SetAttribute("size", size());
		return true;
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