#include <stdexcept>

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

		ImGui::EndDisabled();

		return true;
	}

	void FloatAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() < 1)
			throw std::runtime_error("FloatAttribute::setValue: Not enough values provided.");

		if (values[0].type() != typeid(float))
			throw std::runtime_error("FloatAttribute::setValue: Invalid type for float attribute.");

		m_value = std::any_cast<float>(values[0]);
	}
}
