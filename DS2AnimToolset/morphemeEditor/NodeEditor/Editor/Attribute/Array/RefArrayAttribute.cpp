#include <stdexcept>

#include "RefArrayAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool RefArrayAttribute::editorGUI()
	{
		ImGui::TextUnformatted(m_name.c_str());

		for (auto& value : m_values)
		{
			if (value)
				ImGui::Label(value->getFullName().c_str());
			else
				ImGui::Label("");
		}

		return true;
	}

	void RefArrayAttribute::setValue(const std::vector<std::any>& values)
	{
		m_values.clear();
		m_values.reserve(values.size());

		for (const auto& val : values)
		{
			if (val.type() != typeid(Entity*))
				throw std::runtime_error("RefArrayAttribute::setValue: Invalid type in values array. Expected Entity*.");

			Entity* entity = std::any_cast<Entity*>(val);

			m_values.push_back(entity);
		}
	}
}
