#include <stdexcept>

#include "NodeEditor/NodeEditor.h"
#include "IntAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool IntAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		ImGui::RightAlignedInputInt(m_name.c_str(), &m_value);

		ImGui::EndDisabled();

		return true;
	}

	void IntAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() < 1)
			throw std::runtime_error("IntAttribute::setValue: Insufficient values in values vector.");

		if (values[0].type() != typeid(int))
			throw std::runtime_error("IntAttribute::setValue: Invalid type in values vector. Expected int.");

		m_value = std::any_cast<int>(values[0]);
	}
}
