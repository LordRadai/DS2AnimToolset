#include <stdexcept>

#include "BoolAttribute.h"
#include "imgui/imgui.h"

namespace NodeEditor
{
	bool BoolAttribute::editorGUI()
	{
		ImGui::Checkbox(m_name.c_str(), &m_value);

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
