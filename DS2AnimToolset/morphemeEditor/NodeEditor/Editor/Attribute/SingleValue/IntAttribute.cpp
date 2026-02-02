#include <stdexcept>

#include "IntAttribute.h"
#include "imgui/imgui.h"

namespace NodeEditor
{
	bool IntAttribute::editorGUI()
	{
		ImGui::DragInt(m_name.c_str(), &m_value);

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
