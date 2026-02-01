#include "Attribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	Attribute::Attribute(Entity* owner, const std::string& name, const std::string& type) : Entity(owner->getOwnerEditor(), name),
		m_owner(owner), m_type(type)
	{
	}
}
