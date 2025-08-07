#include "StringArrayAttribute.h"

namespace db
{
	bool StringArrayAttribute::compare(Attribute* other)
	{
		StringArrayAttribute* otherTyped = other->asStringArray();

		if (otherTyped == nullptr)
			throw std::runtime_error("StringArrayAttribute::compare() failed - Other attribute is not a bool array");

		if (otherTyped->size() != this->size())
			return false;

		for (size_t i = 0; i < size(); i++)
		{
			if (m_values[i] != otherTyped->getElement(i))
				return false;
		}

		return true;
	}

	void StringArrayAttribute::assign(Attribute* other)
	{
		StringArrayAttribute* otherComposite = other->asStringArray();

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::assign() failed - Other attribute is not a composite");

		m_values.clear();

		m_values.reserve(otherComposite->size());
		for (size_t i = 0; i < otherComposite->size(); ++i)
			m_values.push_back(otherComposite->getElement(i));
	}

	bool StringArrayAttribute::writeValueXML(int format, SaverXML* saver)
	{
		if (writeStartArrayXML(format, saver))
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

	bool StringArrayAttribute::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("StringArrayAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "stringArray")
		{
			throw std::runtime_error("StringArrayAttribute::readValueXML: Expected type 'stringArray'.");
			return false;
		}
		
		if (!element->getDataAsStringArray(m_values))
		{
			throw std::runtime_error("StringArrayAttribute::readValueXML: Failed to read string array value from XML element.");
			return false;
		}

		return true;
	}

	bool StringArrayAttribute::writeStartArrayXML(int format, SaverXML* saver) const
	{
		m_xmlElement->SetAttribute("size", size());
		return true;
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