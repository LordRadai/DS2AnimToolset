#include <stdexcept>

#include "FloatArrayAttribute.h"
#include "imgui/imgui.h"

namespace NodeEditor
{
	bool FloatArrayAttribute::editorGUI()
	{
		for (size_t i = 0; i < m_values.size(); i++)
		{
			char nameBuffer[32];
			sprintf_s(nameBuffer, "%s%d", m_name.c_str(), (int)i);

			ImGui::InputFloat(nameBuffer, &m_values[i]);
		}

		return true;
	}

	void FloatArrayAttribute::setValue(const std::vector<std::any>& values)
	{
		m_values.clear();
		m_values.reserve(values.size());

		for (const std::any& val : values)
		{
			if (val.type() != typeid(float))
				throw std::runtime_error("FloatArrayAttribute::setValue: Invalid type in values vector. Expected float.");

			m_values.push_back(std::any_cast<float>(val));
		}
	}
}
