#include <stdexcept>

#include "StringAttribute.h"
#include "imgui/imgui.h"

#define TEMP_BUFFER_SIZE 1024

namespace NodeEditor
{
	bool StringAttribute::editorGUI()
	{
		char buffer[TEMP_BUFFER_SIZE];
		strcpy_s(buffer, TEMP_BUFFER_SIZE, m_value.c_str());

		if (ImGui::InputText(m_name.c_str(), buffer, sizeof(buffer)))
			m_value = std::string(buffer);

		return true;
	}

	void StringAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() < 1)
			throw std::runtime_error("StringAttribute::setValue: Not enough values provided.");
		
		if (values[0].type() != typeid(std::string))
			throw std::runtime_error("StringAttribute::setValue: Invalid type provided. Expected std::string.");

		m_value = std::any_cast<std::string>(values[0]);
	}
}