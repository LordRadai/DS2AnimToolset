#include <stdexcept>

#include "ControlParameterAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool ControlParameterAttribute::editorGUI()
	{
		ImGui::TextUnformatted(m_name.c_str());

		ImGui::SameLine();

		if (m_controlParameter)
			ImGui::Label(m_controlParameter->getFullName().c_str());
		else
			ImGui::Label("");

		return true;
	}

	void ControlParameterAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() != 1)
			throw std::runtime_error("ControlParameterAttribute::setValue: Invalid number of values provided.");

		if (values[0].type() != typeid(ControlParameter*))
			throw std::runtime_error("ControlParameterAttribute::setValue: Invalid value type provided.");

		m_controlParameter = std::any_cast<ControlParameter*>(values[0]);
	}
}
