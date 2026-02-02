#include <stdexcept>

#include "NodeEditor/NodeEditor.h"

#include "BoolAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool BoolAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		ImGui::RightAlignedCheckbox(m_name.c_str(), &m_value);

		ImGui::EndDisabled();

		return true;
	}

	void BoolAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() != 1)
			throw std::invalid_argument("BoolAttribute::setValue: Invalid number of values in values vector. Expected 1.");

		if (values[0].type() != typeid(bool))
			throw std::invalid_argument("BoolAttribute::setValue: Invalid type in values vector. Expected bool.");

		m_value = std::any_cast<bool>(values[0]);
	}
}
