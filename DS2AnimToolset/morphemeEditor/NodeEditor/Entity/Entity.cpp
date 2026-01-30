#include "Entity.h"
#include "NodeEditor/NodeEditorBase.h"

namespace NodeEditor
{
	Entity::Entity(NodeEditorBase* editor, const std::string& name) : m_name(name), m_ownerEditor(editor)
	{
		Registry* registry = editor->getRegistry();

		m_id = registry->generateUniqueRuntimeID();
		registry->registerEntity(this);
	}

	Entity::~Entity()
	{
		m_ownerEditor->getRegistry()->unregisterEntity(this);
	}
}
