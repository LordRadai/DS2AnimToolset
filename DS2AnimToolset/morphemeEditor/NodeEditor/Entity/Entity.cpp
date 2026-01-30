#include "Entity.h"
#include "NodeEditor/Registry/Registry.h"

namespace NodeEditor
{
	Entity::Entity(NodeEditorBase* editor, const std::string& name) : m_name(name), m_ownerEditor(editor)
	{
		m_id = Registry::getInstance()->generateUniqueRuntimeID();
		Registry::getInstance()->registerEntity(this);
	}

	Entity::~Entity()
	{
		Registry::getInstance()->unregisterEntity(this);
	}
}
