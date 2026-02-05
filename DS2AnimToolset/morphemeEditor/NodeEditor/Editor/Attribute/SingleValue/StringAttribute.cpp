#include "RCore.h"

#include "NodeEditor/NodeEditor.h"
#include "StringAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

#define TEMP_BUFFER_SIZE 1024

namespace NodeEditor
{
	bool StringAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		ImGui::NamedLabel(m_name.c_str(), m_value.c_str());

		if (!m_helpText.empty())
			ImGui::SetItemTooltip(m_helpText.c_str());

		ImGui::EndDisabled();

		return true;
	}

	void StringAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() < 1)
			INVOKE_PANIC("StringAttribute::setValue: Not enough values provided.");
		
		if (values[0].type() != typeid(std::string))
			INVOKE_PANIC("StringAttribute::setValue: Invalid type provided. Expected std::string.");

		m_value = std::any_cast<std::string>(values[0]);
	}
}