#include <stdexcept>

#include "NodeEditor/NodeEditor.h"
#include "ControlParameterAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool ControlParameterAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		if (m_controlParameter)
			ImGui::NamedLabel(m_name.c_str(), m_controlParameter->getName().c_str());
		else
			ImGui::NamedLabel(m_name.c_str(), "");

		if (!m_helpText.empty())
			ImGui::SetItemTooltip(m_helpText.c_str());

		ImGui::EndDisabled();

		return true;
	}

	void ControlParameterAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() != 1)
			INVOKE_PANIC("ControlParameterAttribute::setValue: Invalid number of values provided.");

		if (values[0].type() != typeid(ControlParameter*))
			INVOKE_PANIC("ControlParameterAttribute::setValue: Invalid value type provided.");

		m_controlParameter = std::any_cast<ControlParameter*>(values[0]);
	}
}
