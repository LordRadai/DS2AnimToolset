#include "CompoundAttribute.h"

namespace db
{
	bool CompoundAttribute::readValueXML(int format, XMLElement* element, LoaderXML* loader)
	{
		if (element->getChildCount() != m_attributes.size())
		{
			throw std::runtime_error("CompoundAttribute::readValueXML: XML element child count does not match attribute count.");
			return false;
		}

		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			db::Attribute* attr = m_attributes[i];
			XMLElement* childElement = element->findChild(m_attributes[i]->getName());

			if (!childElement)
				throw std::runtime_error("CompoundAttribute::readValueXML: Child element not found for attribute '" + attr->getName() + "'.");

			attr->readValueXML(format, childElement, loader);
		}
	}

	bool CompoundAttribute::writeValueXML(int format, SaverXML* saver)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			if (!m_attributes[i]->writeXML(format, saver))
				return false;
		}

		return true;
	}
}
