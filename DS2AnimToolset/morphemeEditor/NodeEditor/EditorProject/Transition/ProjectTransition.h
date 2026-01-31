#pragma once
#include "NodeEditor/EditorProject/Entity/ProjectEntity.h"

namespace NodeEditor
{
	namespace Project
	{
		class ProjectNode;

		class ProjectTransition : public ProjectEntity
		{
		public:
			ProjectTransition(EditorProject* project, tinyxml2::XMLElement* xmlElement);
			virtual ~ProjectTransition() override {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement) override;

			const int getSourceNodeID() const;
			void setSourceNodeID(int sourceNodeID);

			const int getDestinationNodeID() const;
			void setDestinationNodeID(int targetNodeID);
		};
	}
}