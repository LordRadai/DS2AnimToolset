#pragma once
#include <string>
#include <vector>
#include <any>
#include "NodeEditor/Editor/Entity/Entity.h"

namespace NodeEditor
{
	class Attribute : public Entity
	{
	protected:
		Entity* m_owner;
		std::string m_type;

	public:
		Attribute(Entity* owner, const std::string& name, const std::string& type);

		virtual ~Attribute() override {}
		virtual const std::string getFullName() const override { return m_owner->getFullName() + "." + m_name; }
		virtual bool editorGUI() override { return true; }
		virtual void setValue(const std::vector<std::any>& values) {}

		Entity* getOwnerEntity() const { return m_owner; }
		const std::string& getName() const { return m_name; }
		const std::string& getType() const { return m_type; }

		static Attribute* createAttribute(Entity* owner, const std::string& name, const std::string& type);
	};
}
