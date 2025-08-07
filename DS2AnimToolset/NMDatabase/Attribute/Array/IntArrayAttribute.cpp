#include "IntArrayAttribute.h"

namespace db
{
	bool IntArrayAttribute::compare(Attribute* other)
	{
		IntArrayAttribute* otherTyped = other->asIntArray();

		if (otherTyped == nullptr)
			throw std::runtime_error("IntArrayAttribute::compare() failed - Other attribute is not a bool array");

		if (otherTyped->size() != this->size())
			return false;

		for (size_t i = 0; i < size(); i++)
		{
			if (m_values[i] != otherTyped->getElement(i))
				return false;
		}

		return true;
	}

	void IntArrayAttribute::assign(Attribute* other)
	{
		IntArrayAttribute* otherComposite = other->asIntArray();

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::assign() failed - Other attribute is not a composite");

		m_values.clear();

		m_values.reserve(otherComposite->size());
		for (size_t i = 0; i < otherComposite->size(); ++i)
			m_values.push_back(otherComposite->getElement(i));
	}

	bool IntArrayAttribute::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("IntArrayAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "intArray")
		{
			throw std::runtime_error("IntArrayAttribute::readValueXML: Expected type 'intArray'.");
			return false;
		}
		
		if (!element->getDataAsIntArray(m_values))
		{
			throw std::runtime_error("IntArrayAttribute::readValueXML: Failed to read int array value from XML element.");
			return false;
		}

		return true;
	}

	bool IntArrayAttribute::writeValueXML(int format, SaverXML* saver)
	{
		if (writeStartArrayXML(format, saver))
		{
			for (size_t i = 0; i < m_values.size(); ++i)
				m_xmlElement->InsertNewChildElement("fill")->SetText(m_values[i]);

			return true;
		}

		return false;
	}

	void IntArrayAttribute::removeAt(int idx)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("IntArrayAttribute::removeAt() - Index out of range");

		m_values.erase(m_values.begin() + idx);
	}

	bool IntArrayAttribute::writeStartArrayXML(int format, SaverXML* saver) const
	{
		m_xmlElement->SetAttribute("size", size());
		return true;
	}

	void IntArrayAttribute::insert(int idx, int value)
	{
		if (idx < 0 || idx > static_cast<int>(m_values.size()))
			throw std::out_of_range("IntArrayAttribute::insert - Index out of range");

		m_values.insert(m_values.begin() + idx, value);
	}

	void IntArrayAttribute::setElement(int idx, int value)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("IntArrayAttribute::setElement - Index out of range");
		m_values[idx] = value;
	}

	int IntArrayAttribute::getElement(int idx) const
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("IntArrayAttribute::getElement - Index out of range");
		return m_values[idx];
	}
}