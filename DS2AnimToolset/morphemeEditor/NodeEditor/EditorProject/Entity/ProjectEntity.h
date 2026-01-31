#pragma once
#include "RCore.h"

namespace NodeEditor
{
	namespace Project
	{
		class ProjectEntity
		{
		protected:
			tinyxml2::XMLElement* m_xmlElement;
		public:
			ProjectEntity(tinyxml2::XMLElement* xmlElement, const std::string& elemName);
			virtual ~ProjectEntity() {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement);

			tinyxml2::XMLElement* getXMLElement() const { return m_xmlElement; }

			const std::string getName() const;
			void setName(const std::string& name);
		};
	}
}
