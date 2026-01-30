#pragma once
#include "NodeEditor/EditorProject/Entity/Entity.h"

namespace NodeEditor
{
	namespace Project
	{
		class ControlParameter : public Entity
		{
		public:
			ControlParameter(tinyxml2::XMLElement* xmlElement) : Entity(xmlElement, "ControlParameter") {}
			virtual ~ControlParameter() override {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement) override;

			const std::string getType() const;
			void setType(const std::string& type);

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
