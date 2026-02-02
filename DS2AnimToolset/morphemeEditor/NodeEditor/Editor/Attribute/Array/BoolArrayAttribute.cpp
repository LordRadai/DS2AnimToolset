#include <stdexcept>

#include "NodeEditor/NodeEditor.h"
#include "BoolArrayAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool BoolArrayAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		for (size_t i = 0; i < m_values.size(); i++)
		{
			char nameBuffer[32];
			sprintf_s(nameBuffer, "%s%d", m_name.c_str(), (int)i);

			bool value = m_values[i];

			if (ImGui::RightAlignedCheckbox(nameBuffer, &value))
				m_values[i] = value;
		}

		ImGui::EndDisabled();

		return true;
	}

	void BoolArrayAttribute::setValue(const std::vector<std::any>& values)
	{
		m_values.clear();
		m_values.reserve(values.size());

		for (const std::any& val : values)
		{
			if (val.type() != typeid(bool))
				throw std::invalid_argument("BoolArrayAttribute::setValue: Invalid type in values vector. Expected bool.");

			m_values.push_back(std::any_cast<bool>(val));
		}
	}
}
