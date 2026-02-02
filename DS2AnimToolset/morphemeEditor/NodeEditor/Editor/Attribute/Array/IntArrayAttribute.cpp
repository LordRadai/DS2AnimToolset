#include <stdexcept>

#include "IntArrayAttribute.h"
#include "imgui/imgui.h"

namespace NodeEditor
{
	bool IntArrayAttribute::editorGUI()
	{
		for (size_t i = 0; i < m_values.size(); i++)
		{
			char nameBuffer[32];
			sprintf_s(nameBuffer, "%s%d", m_name.c_str(), (int)i);

			ImGui::InputInt(nameBuffer, &m_values[i]);
		}

		return true;
	}

	void IntArrayAttribute::setValue(const std::vector<std::any>& values)
	{
		m_values.clear();
		m_values.reserve(values.size());

		for (const std::any& value : values)
		{
			if (value.type() != typeid(int))
				throw std::runtime_error("IntArrayAttribute::setValue: Invalid type in values vector. Expected int.");

			m_values.push_back(std::any_cast<int>(value));
		}
	}
}
