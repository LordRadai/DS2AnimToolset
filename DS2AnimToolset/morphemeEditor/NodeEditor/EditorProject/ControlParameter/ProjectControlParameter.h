#pragma once
#include "NodeEditor/EditorProject/Entity/ProjectEntity.h"

namespace NodeEditor
{
	namespace Project
	{
		class ProjectControlParameter : public ProjectEntity
		{
		public:
			ProjectControlParameter(EditorProject* project, tinyxml2::XMLElement* xmlElement) : ProjectEntity(project, xmlElement, "ControlParameter") {}
			virtual ~ProjectControlParameter() override {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement) override;

			const std::string getType() const;
			void setType(const std::string& type);

			int getControlParameterID() const;
			void setControlParameterID(int id);

			template<typename T>
			T getDefaultValue() const
			{
				static_assert(sizeof(T) == 0, "getDefaultValue not implemented for this type");
			}

			template<typename T>
			void setDefaultValue(const T& value)
			{
				static_assert(sizeof(T) == 0, "setDefaultValue not implemented for this type");
			}
		};
	}
}
