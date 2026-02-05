#include "RCore.h"

#include "NodeEditor/NodeEditor.h"
#include "IntArrayAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool IntArrayAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		for (size_t i = 0; i < m_values.size(); i++)
		{
			char nameBuffer[32];
			sprintf_s(nameBuffer, "%s%d", m_name.c_str(), (int)i);

			ImGui::RightAlignedInputInt(nameBuffer, &m_values[i]);

			if (!m_helpText.empty())
				ImGui::SetItemTooltip(m_helpText.c_str());
		}

		ImGui::EndDisabled();

		return true;
	}

	void IntArrayAttribute::setValue(const std::vector<std::any>& values)
	{
		m_values.clear();
		m_values.reserve(values.size());

		for (const std::any& value : values)
		{
			if (value.type() != typeid(int))
				INVOKE_PANIC("IntArrayAttribute::setValue: Invalid type in values vector. Expected int.");

			m_values.push_back(std::any_cast<int>(value));
		}
	}
}
