#include "RCore.h"

#include "NodeEditor/NodeEditor.h"
#include "FloatAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool FloatAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		ImGui::RightAlignedInputFloat(m_name.c_str(), &m_value);

		if (!m_helpText.empty())
			ImGui::SetItemTooltip(m_helpText.c_str());

		ImGui::EndDisabled();

		return true;
	}

	void FloatAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() < 1)
			INVOKE_PANIC("FloatAttribute::setValue: Not enough values provided.");

		if (values[0].type() != typeid(float))
			INVOKE_PANIC("FloatAttribute::setValue: Invalid type for float attribute.");

		m_value = std::any_cast<float>(values[0]);
	}
}
