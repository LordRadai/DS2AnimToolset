#include "Transition.h"

namespace NodeEditor
{
	namespace Project
	{
		bool Transition::loadFromXMLElement(tinyxml2::XMLElement* xmlElement)
		{
			if (!Entity::loadFromXMLElement(xmlElement))
				return false;

			return true;
		}

		const int Transition::getSourceNodeID() const
		{
			return m_xmlElement->IntAttribute("sourceNodeID");
		}

		void Transition::setSourceNodeID(int sourceNodeID)
		{
			m_xmlElement->SetAttribute("sourceNodeID", sourceNodeID);
		}

		const int Transition::getDestinationNodeID() const
		{
			return m_xmlElement->IntAttribute("destinationNodeID");
		}

		void Transition::setDestinationNodeID(int targetNodeID)
		{
			m_xmlElement->SetAttribute("destinationNodeID", targetNodeID);
		}
	}
}
