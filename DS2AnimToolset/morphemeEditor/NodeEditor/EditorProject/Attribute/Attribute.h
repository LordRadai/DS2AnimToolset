#pragma once
#include "NodeEditor/EditorProject/Entity/Entity.h"

namespace NodeEditor
{
	namespace Project
	{
		class Attribute : public Entity
		{
			Entity* m_parent;
		public:
			Attribute(Entity* parent, tinyxml2::XMLElement* xmlElement) : Entity(xmlElement, "Attribute"), m_parent(parent) {}
			virtual ~Attribute() override {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement) override;

			Entity* getParent() const { return m_parent; }
		};
	}
}
