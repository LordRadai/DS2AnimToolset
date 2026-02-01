#pragma once
#include <string>
#include "NodeEditor/Editor/Entity/Entity.h"

namespace NodeEditor
{
	class Attribute : public Entity
	{
	private:
		Entity* m_owner;
		std::string m_type;

	public:
		Attribute(Entity* owner, const std::string& name, const std::string& type);

		virtual ~Attribute() override {}

		Entity* getOwnerEntity() const { return m_owner; }
		const std::string& getName() const { return m_name; }
		const std::string& getType() const { return m_type; }
	};
}
