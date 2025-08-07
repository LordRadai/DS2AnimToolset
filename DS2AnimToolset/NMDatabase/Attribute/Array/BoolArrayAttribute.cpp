#include "BoolArrayAttribute.h"

namespace db
{
	bool BoolArrayAttribute::compare(Attribute* other)
	{
		BoolArrayAttribute* otherTyped = other->asBoolArray();

		if (otherTyped == nullptr)
			throw std::runtime_error("BoolArrayAttribute::compare() failed - Other attribute is not a bool array");

		if (otherTyped->size() != this->size())
			return false;

		for (size_t i = 0; i < size(); i++)
		{
			if (m_values[i] != otherTyped->getElement(i))
				return false;
		}

		return true;
	}

	void BoolArrayAttribute::assign(Attribute* other)
	{
		BoolArrayAttribute* otherComposite = other->asBoolArray();

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::assign() failed - Other attribute is not a composite");

		m_values.clear();

		m_values.reserve(otherComposite->size());
		for (size_t i = 0; i < otherComposite->size(); ++i)
			m_values.push_back(otherComposite->getElement(i));
	}

	bool BoolArrayAttribute::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("BoolArrayAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "boolArray")
		{
			throw std::runtime_error("BoolArrayAttribute::readValueXML: Expected type 'boolArray'.");
			return false;
		}

		if (!element->getDataAsBoolArray(m_values))
		{
			throw std::runtime_error("BoolArrayAttribute::readValueXML: Failed to read bool array value from XML element.");
			return false;
		}

		return true;
	}

	bool BoolArrayAttribute::writeValueXML(int format, SaverXML* saver)
	{
		if (writeStartArrayXML(format, saver))
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

	bool BoolArrayAttribute::writeStartArrayXML(int format, SaverXML* saver) const
	{
		m_xmlElement->SetAttribute("size", size());
		return true;
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