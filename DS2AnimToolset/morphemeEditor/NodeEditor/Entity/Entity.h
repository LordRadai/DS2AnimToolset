#pragma once

namespace NodeEditor
{
	class Entity
	{
	protected:
		int m_id;
	public:
		Entity();
		~Entity();

		int getID() const { return m_id; }
	};
}