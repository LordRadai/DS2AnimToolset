#pragma once
#include "NodeEditor/EditorProject/Entity/ProjectEntity.h"

namespace NodeEditor
{
	namespace Project
	{
		class ProjectTransition : public ProjectEntity
		{
		public:
			ProjectTransition(tinyxml2::XMLElement* xmlElement) : ProjectEntity(xmlElement, "Transition") {}
			virtual ~ProjectTransition() override {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement) override;

			const int getSourceNodeID() const;
			void setSourceNodeID(int sourceNodeID);

			const int getDestinationNodeID() const;
			void setDestinationNodeID(int targetNodeID);
		};
	}
}