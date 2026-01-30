#pragma once
#include <string>

namespace NodeEditor
{
	class NodeEditorBase;

	class Entity
	{
	protected:
		NodeEditorBase* m_ownerEditor = nullptr;
		int m_id;
		std::string m_name;
	public:
		Entity(NodeEditorBase* editor, const std::string& name);
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