#pragma once
#include "NodeEditor/EditorProject/Entity/ProjectEntity.h"

namespace NodeEditor
{
	namespace Project
	{
		class ProjectAttribute : public ProjectEntity
		{
			ProjectEntity* m_parent;
		public:
			ProjectAttribute(ProjectEntity* parent, tinyxml2::XMLElement* xmlElement) : ProjectEntity(xmlElement, "Attribute"), m_parent(parent) {}
			virtual ~ProjectAttribute() override {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement) override;

			ProjectEntity* getParent() const { return m_parent; }
		};
	}
}
