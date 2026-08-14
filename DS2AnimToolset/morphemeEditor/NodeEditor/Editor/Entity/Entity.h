#pragma once
#include <string>

namespace NodeEditor
{
	class Editor;

	class Entity
	{
	protected:
		Editor* m_ownerEditor = nullptr;
		int m_id;
		std::string m_name;
	public:
		Entity(Editor* editor, const std::string& name);

		virtual ~Entity();
		virtual void draw() {}
		virtual bool editorGUI() { return false; }
		virtual const std::string& getName() const { return m_name; }
		virtual void setName(const std::string& name) { m_name = name; }
		virtual const std::string getFullName() const { return m_name; }
		virtual std::string exportLua() { return ""; }

		Editor* getOwnerEditor() const { return m_ownerEditor; }
		int getID() const { return m_id; }

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