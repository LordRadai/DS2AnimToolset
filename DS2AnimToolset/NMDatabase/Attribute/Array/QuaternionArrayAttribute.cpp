#include "QuaternionArrayAttribute.h"

namespace db
{
	bool QuaternionArrayAttribute::compare(Attribute* other)
	{
		QuaternionArrayAttribute* otherTyped = other->asQuaternionArray();

		if (otherTyped == nullptr)
			throw std::runtime_error("QuaternionArrayAttribute::compare() failed - Other attribute is not a bool array");

		if (otherTyped->size() != this->size())
			return false;

		for (size_t i = 0; i < size(); i++)
		{
			if (m_values[i] != otherTyped->getElement(i))
				return false;
		}

		return true;
	}

	void QuaternionArrayAttribute::assign(Attribute* other)
	{
		QuaternionArrayAttribute* otherComposite = other->asQuaternionArray();

		if (otherComposite == nullptr)
			throw std::runtime_error("CompositeAttribute::assign() failed - Other attribute is not a composite");

		m_values.clear();

		m_values.reserve(otherComposite->size());
		for (size_t i = 0; i < otherComposite->size(); ++i)
			m_values.push_back(otherComposite->getElement(i));
	}

	bool QuaternionArrayAttribute::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("QuaternionArrayAttribute::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "quaternionArray")
		{
			throw std::runtime_error("QuaternionArrayAttribute::readValueXML: Expected type 'quaternionArray'.");
			return false;
		}
		
		if (!element->getDataAsQuatArray(m_values))
		{
			throw std::runtime_error("QuaternionArrayAttribute::readValueXML: Failed to read quaternion array from XML element.");
			return false;
		}

		return true;
	}

	bool QuaternionArrayAttribute::writeValueXML(int format, SaverXML* saver)
	{
		if (writeStartArrayXML(format, saver))
		{
			for (size_t i = 0; i < m_values.size(); ++i)
			{
				//m_xmlElement->InsertNewChildElement("elem")->SetText(m_values[i]);
			}

			return true;
		}

		return false;
	}

	void QuaternionArrayAttribute::removeAt(int idx)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("QuaternionArrayAttribute::removeAt() - Index out of range");

		m_values.erase(m_values.begin() + idx);
	}

	bool QuaternionArrayAttribute::writeStartArrayXML(int format, SaverXML* saver) const
	{
		if (size() == 0)
			return false;

		m_xmlElement->SetAttribute("size", size());
		return true;
	}

	void QuaternionArrayAttribute::insert(int idx, NMP::Quat value)
	{
		if (idx < 0 || idx > static_cast<int>(m_values.size()))
			throw std::out_of_range("QuaternionArrayAttribute::insert - Index out of range");

		m_values.insert(m_values.begin() + idx, value);
	}

	void QuaternionArrayAttribute::setElement(int idx, NMP::Quat value)
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("QuaternionArrayAttribute::setElement - Index out of range");
		m_values[idx] = value;
	}

	NMP::Quat QuaternionArrayAttribute::getElement(int idx) const
	{
		if (idx < 0 || idx >= static_cast<int>(m_values.size()))
			throw std::out_of_range("QuaternionArrayAttribute::getElement - Index out of range");
		return m_values[idx];
	}
}