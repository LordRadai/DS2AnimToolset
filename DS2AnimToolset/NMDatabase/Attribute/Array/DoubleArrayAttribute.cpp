#include "DoubleArrayAttribute.h"

namespace db
{
	bool DoubleArrayAttribute::compare(Attribute* other)
	{
		DoubleArrayAttribute* otherTyped = other->asDoubleArray();

		if (otherTyped == nullptr)
			throw std::runtime_error("DoubleArrayAttribute::compare() failed - Other attribute is not a bool array");

		if (otherTyped->size() != this->size())
			return false;

		for (size_t i = 0; i < size(); i++)
		{
			if (m_values[i] != otherTyped->getElement(i))
				return false;
		}

		return true;
	}

	void DoubleArrayAttribute::assign(Attribute* other)
	{
		DoubleArrayAttribute* otherComposite = other->asDoubleArray();

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::assign() failed - Other attribute is not a composite");

		m_values.clear();

		m_values.reserve(otherComposite->size());
		for (size_t i = 0; i < otherComposite->size(); ++i)
			m_values.push_back(otherComposite->getElement(i));
	}

	bool DoubleArrayAttribute::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("DoubleArrayAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");

		if (!m_type.empty() && m_type != "doubleArray")
		{
			throw std::runtime_error("DoubleArrayAttribute::readValueXML: Expected type 'doubleArray'.");
			return false;
		}
		
		if (!element->getDataAsDoubleArray(m_values))
		{
			throw std::runtime_error("DoubleArrayAttribute::readValueXML: Failed to read double array from XML element.");
			return false;
		}

		return true;
	}

	bool DoubleArrayAttribute::writeValueXML(int format, SaverXML* saver)
	{
		if (writeStartArrayXML(format, saver))
		{
			for (size_t i = 0; i < m_values.size(); ++i)
				m_xmlElement->InsertNewChildElement("fill")->SetText(m_values[i]);

			return true;
		}

		return false;
	}

	void DoubleArrayAttribute::removeAt(int idx)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("DoubleArrayAttribute::removeAt() - Index out of range");

		m_values.erase(m_values.begin() + idx);
	}

	bool DoubleArrayAttribute::writeStartArrayXML(int format, SaverXML* saver) const
	{
		m_xmlElement->SetAttribute("size", size());
		return true;
	}

	void DoubleArrayAttribute::insert(int idx, double value)
	{
		if (idx < 0 || idx > static_cast<int>(m_values.size()))
			throw std::out_of_range("DoubleArrayAttribute::insert - Index out of range");

		m_values.insert(m_values.begin() + idx, value);
	}

	void DoubleArrayAttribute::setElement(int idx, double value)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("DoubleArrayAttribute::setElement - Index out of range");
		m_values[idx] = value;
	}

	double DoubleArrayAttribute::getElement(int idx) const
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("DoubleArrayAttribute::getElement - Index out of range");
		return m_values[idx];
	}
}