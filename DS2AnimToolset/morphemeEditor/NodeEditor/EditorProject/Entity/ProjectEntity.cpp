#include "ProjectEntity.h"
#include "NodeEditor/EditorProject/EditorProject.h"

namespace NodeEditor
{
	namespace Project
	{
		ProjectEntity::ProjectEntity(EditorProject* project, tinyxml2::XMLElement* xmlElement, const std::string& elemName) : m_project(project), m_xmlElement(nullptr)
		{
			m_xmlElement = xmlElement->InsertNewChildElement(elemName.c_str());
		}

		bool ProjectEntity::loadFromXMLElement(tinyxml2::XMLElement* xmlElement)
		{
			if (xmlElement)
			{
				m_xmlElement = xmlElement;
				return true;
			}

			return false;
		}

		const std::string ProjectEntity::getName() const
		{
			if (m_xmlElement)
			{
				const char* nameAttr = m_xmlElement->Attribute("name");

				if (nameAttr)
					return std::string(nameAttr);
			}

			return "";
		}

		void ProjectEntity::setName(const std::string& name)
		{
			if (m_xmlElement)
				m_xmlElement->SetAttribute("name", name.c_str());
		}
	}
}
