#include "Entity.h"
#include "NodeEditor/Registry/Registry.h"

namespace NodeEditor
{
	Entity::Entity()
	{
		m_id = Registry::getInstance()->generateUniqueRuntimeID();
		Registry::getInstance()->registerEntity(this);
	}

	Entity::~Entity()
	{
		Registry::getInstance()->unregisterEntity(this);
	}
}
