#pragma once
#include <string>
#include "NodeEditor/EditorProject/Entity/ProjectEntity.h"

namespace NodeEditor
{
	class NodeEditor;

	class Entity
	{
	protected:
		NodeEditor* m_ownerEditor = nullptr;
		int m_id;
		std::string m_name;
	public:
		Entity(NodeEditor* editor, const std::string& name);
		Entity(NodeEditor* editor, Project::ProjectEntity* projectEntity) : Entity(editor, projectEntity->getName()) {}

		virtual ~Entity();
		virtual void draw() = 0;

		int getID() const { return m_id; }
		const std::string& getName() const { return m_name; }

		template<typename T>
		bool isOfType() const
		{
			return dynamic_cast<const T*>(this) != nullptr;
		}

		template<typename T>
		T* asType()
		{
			return dynamic_cast<T*>(this);
		}
	};
}