#include "Attribute.h"

namespace NodeEditor
{
	namespace Project
	{
		bool Attribute::loadFromXMLElement(tinyxml2::XMLElement* xmlElement)
		{
			if (xmlElement)
			{
				m_xmlElement = xmlElement;
				return true;
			}

			return false;
		}
	}
}
