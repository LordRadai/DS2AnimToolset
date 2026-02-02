#include <stdexcept>

#include "FloatAttribute.h"
#include "imgui/imgui.h"

namespace NodeEditor
{
	bool FloatAttribute::editorGUI()
	{
		ImGui::DragFloat(m_name.c_str(), &m_value);

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
