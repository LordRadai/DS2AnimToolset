#include "RCore.h"

#include "NodeEditor/NodeEditor.h"
#include "RefAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool RefAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		if (m_value)
			ImGui::NamedLabel(m_name.c_str(), m_value->getFullName().c_str());
		else 
			ImGui::NamedLabel(m_name.c_str(), "");

		if (!m_helpText.empty())
			ImGui::SetItemTooltip(m_helpText.c_str());

		ImGui::EndDisabled();

		return true;
	}

	void RefAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() != 1)
			INVOKE_PANIC("RefAttribute::setValue: Invalid number of values provided.");
		
		if (values[0].type() != typeid(Entity*))
			INVOKE_PANIC("RefAttribute::setValue: Invalid value type provided.");

		m_value = std::any_cast<Entity*>(values[0]);
	}
}