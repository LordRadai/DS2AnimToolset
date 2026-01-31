#pragma once
#include "RCore.h"

namespace NodeEditor
{
	class EditorProject;

	namespace Project
	{
		class ProjectEntity
		{
		protected:
			EditorProject* m_project;
			tinyxml2::XMLElement* m_xmlElement;
		public:
			ProjectEntity(EditorProject* project, tinyxml2::XMLElement* xmlElement, const std::string& elemName);
			virtual ~ProjectEntity() {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement);

			EditorProject* getProject() const { return m_project; }
			tinyxml2::XMLElement* getXMLElement() const { return m_xmlElement; }

			const std::string getName() const;
			void setName(const std::string& name);
		};
	}
}
