#include <stdexcept>

#include "RefAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool RefAttribute::editorGUI()
	{
		ImGui::TextUnformatted(m_name.c_str());

		if (m_value)
			ImGui::Label(m_value->getFullName().c_str());
		else 
			ImGui::Label("");

		return true;
	}

	void RefAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() != 1)
			throw std::runtime_error("RefAttribute::setValue: Invalid number of values provided.");
		
		if (values[0].type() != typeid(Entity*))
			throw std::runtime_error("RefAttribute::setValue: Invalid value type provided.");

		m_value = std::any_cast<Entity*>(values[0]);
	}
}