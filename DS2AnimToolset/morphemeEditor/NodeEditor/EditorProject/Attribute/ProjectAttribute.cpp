#include "ProjectAttribute.h"

namespace NodeEditor
{
	namespace Project
	{
		bool ProjectAttribute::loadFromXMLElement(tinyxml2::XMLElement* xmlElement)
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
