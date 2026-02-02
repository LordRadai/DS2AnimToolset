#include <stdexcept>

#include "StringArrayAttribute.h"
#include "imgui/imgui.h"

#define TEMP_BUFFER_SIZE 1024

namespace NodeEditor
{
	bool StringArrayAttribute::editorGUI()
	{
		for (size_t i = 0; i < m_values.size(); i++)
		{
			char nameBuffer[32];
			sprintf_s(nameBuffer, "%s%d", m_name.c_str(), (int)i);

			char buffer[TEMP_BUFFER_SIZE];
			strcpy_s(buffer, TEMP_BUFFER_SIZE, m_values[i].c_str());

			if (ImGui::InputText(m_name.c_str(), buffer, sizeof(buffer)))
				m_values[i] = std::string(buffer);
		}

		return true;
	}

	void StringArrayAttribute::setValue(const std::vector<std::any>& values)
	{
		m_values.clear();
		m_values.reserve(values.size());

		for (const std::any& value : values)
		{
			if (value.type() != typeid(std::string))
				throw std::runtime_error("StringArrayAttribute::setValue: Invalid type in values vector. Expected std::string.");

			m_values.push_back(std::any_cast<std::string>(value));
		}
	}
}
