#include "ProjectTransition.h"

namespace NodeEditor
{
	namespace Project
	{
		bool ProjectTransition::loadFromXMLElement(tinyxml2::XMLElement* xmlElement)
		{
			if (!ProjectEntity::loadFromXMLElement(xmlElement))
				return false;

			return true;
		}

		const int ProjectTransition::getSourceNodeID() const
		{
			return m_xmlElement->IntAttribute("sourceNodeID");
		}

		void ProjectTransition::setSourceNodeID(int sourceNodeID)
		{
			m_xmlElement->SetAttribute("sourceNodeID", sourceNodeID);
		}

		const int ProjectTransition::getDestinationNodeID() const
		{
			return m_xmlElement->IntAttribute("destinationNodeID");
		}

		void ProjectTransition::setDestinationNodeID(int targetNodeID)
		{
			m_xmlElement->SetAttribute("destinationNodeID", targetNodeID);
		}
	}
}
