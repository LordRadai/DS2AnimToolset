#include "AttributeArray.h"
#include "../Factory/AttributeFactory.h"

namespace db
{
	bool AttributeArray::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		if (!element)
			return false;

		m_xmlElement = element->getXmlElement();
		m_name = element->getName();

		if (!element->hasAttribute("type"))
			throw std::runtime_error("AttributeArray::readValueXML: Missing 'type' attribute in XML element.");

		m_type = element->getAttribute("type");
		if (!m_type.empty() && m_type != "attributeArray")
		{
			throw std::runtime_error("AttributeArray::readValueXML: Expected type 'attributeArray'.");
			return false;
		}

		for (size_t i = 0; i < element->getNumChildren(); i++)
		{
			XMLElement* child = element->getChild(i);
			Attribute* attr = AttributeFactory::makeAttributeFromTypeString(child->getAttribute("type"));

			if (!attr)
				throw std::runtime_error("AttributeArray::readValueXML: Unknown attribute type '" + std::string(child->getAttribute("type")) + "'.");

			attr->readValueXML(format, child, loader);
			this->add(attr);
		}
	}

	bool AttributeArray::writeValueXML(int format, SaverXML* saver)
	{
		if (writeStartArrayXML(format, saver))
		{
			m_xmlElement->SetAttribute("elemType", getElemType().c_str());

			for (size_t i = 0; i < m_attributes.size(); ++i)
			{
				if (!m_attributes[i]->writeXML(format, saver))
					return false;
			}

			return true;
		}

		return false;
	}

	bool AttributeArray::writeStartArrayXML(int format, SaverXML* saver) const
	{
		m_xmlElement->SetAttribute("size", size());
		return true;
	}
}
