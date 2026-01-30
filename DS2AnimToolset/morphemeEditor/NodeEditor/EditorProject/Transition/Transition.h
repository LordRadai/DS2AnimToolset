#pragma once
#include "NodeEditor/EditorProject/Entity/Entity.h"

namespace NodeEditor
{
	namespace Project
	{
		class Transition : public Entity
		{
		public:
			Transition(tinyxml2::XMLElement* xmlElement) : Entity(xmlElement, "Transition") {}
			virtual ~Transition() override {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement) override;

			const int getSourceNodeID() const;
			void setSourceNodeID(int sourceNodeID);

			const int getDestinationNodeID() const;
			void setDestinationNodeID(int targetNodeID);
		};
	}
}