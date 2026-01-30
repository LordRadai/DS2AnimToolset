#pragma once
#include "RCore.h"

namespace NodeEditor
{
	namespace Project
	{
		class Entity
		{
		protected:
			tinyxml2::XMLElement* m_xmlElement;
		public:
			Entity(tinyxml2::XMLElement* xmlElement, const std::string& elemName);
			virtual ~Entity() {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement);

			tinyxml2::XMLElement* getXMLElement() const { return m_xmlElement; }

			const std::string getName() const;
			void setName(const std::string& name);
		};
	}
}
