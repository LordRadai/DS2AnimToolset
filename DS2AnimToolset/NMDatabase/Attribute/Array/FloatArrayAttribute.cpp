#include "FloatArrayAttribute.h"

namespace db
{
	bool FloatArrayAttribute::compare(Attribute* other)
	{
		FloatArrayAttribute* otherTyped = other->asFloatArray();

		if (otherTyped == nullptr)
			throw std::runtime_error("FloatArrayAttribute::compare() failed - Other attribute is not a bool array");

		if (otherTyped->size() != this->size())
			return false;

		for (size_t i = 0; i < size(); i++)
		{
			if (m_values[i] != otherTyped->getElement(i))
				return false;
		}

		return true;
	}

	void FloatArrayAttribute::assign(Attribute* other)
	{
		FloatArrayAttribute* otherComposite = other->asFloatArray();

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::assign() failed - Other attribute is not a composite");

		m_values.clear();

		m_values.reserve(otherComposite->size());
		for (size_t i = 0; i < otherComposite->size(); ++i)
			m_values.push_back(otherComposite->getElement(i));
	}

	bool FloatArrayAttribute::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("FloatArrayAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");

		if (!m_type.empty() && m_type != "floatArray")
		{
			throw std::runtime_error("FloatArrayAttribute::readValueXML: Expected type 'floatArray'.");
			return false;
		}

		if (!element->hasAttribute("size"))
			throw std::runtime_error("FloatArrayAttribute::readValueXML: Missing 'size' attribute in XML element.");
		
		if (!element->getDataAsFloatArray(m_values))
		{
			throw std::runtime_error("FloatArrayAttribute::readValueXML: Failed to read float array from XML element.");
			return false;
		}

		return true;
	}

	bool FloatArrayAttribute::writeValueXML(int format, SaverXML* saver)
	{
		if (writeStartArrayXML(format, saver))
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

	bool FloatArrayAttribute::writeStartArrayXML(int format, SaverXML* saver) const
	{
		m_xmlElement->SetAttribute("size", size());
		return true;
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