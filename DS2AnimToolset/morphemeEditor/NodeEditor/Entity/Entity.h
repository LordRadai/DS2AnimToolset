#pragma once
#include <string>

namespace NodeEditor
{
	class Entity
	{
	protected:
		int m_id;
		std::string m_name;
	public:
		Entity(const std::string& name);
		virtual ~Entity();
		virtual void draw() = 0;

		int getID() const { return m_id; }
		const std::string& getName() const { return m_name; }
	};
}