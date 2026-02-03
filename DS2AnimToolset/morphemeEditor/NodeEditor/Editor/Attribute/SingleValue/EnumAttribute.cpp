#include <stdexcept>

#include "NodeEditor/NodeEditor.h"
#include "EnumAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace 
NodeEditor
{
	bool EnumAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		const char** optionsArray = new char*[m_enumOptions.size()];

		for (size_t i = 0; i < m_enumOptions.size(); i++)
			optionsArray[i] = m_enumOptions[i].c_str();

		if (m_enumOptions.empty())
			ImGui::NamedLabel(m_name.c_str(), "No options");
		else
			ImGui::RightAlignedCombo(m_name.c_str(), &m_value, optionsArray, m_enumOptions.size());

		delete[] optionsArray;

		ImGui::EndDisabled();

		return true;
	}

	void EnumAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() != 1)
			throw std::runtime_error("EnumAttribute::setValue: Invalid number of values provided.");

		if (values[0].type() == typeid(int))
		{
			if (std::any_cast<int>(values[0]) >= getNumOptions())
				throw std::runtime_error("EnumAttribute::setValue: Enum value out of range.");

			m_value = std::any_cast<int>(values[0]);
		}
		else if (values[0].type() == typeid(std::string))
			m_value = getValueForString(std::any_cast<std::string>(values[0]));
		else
			throw std::runtime_error("EnumAttribute::setValue: Invalid value type provided.");
	}

	std::string EnumAttribute::getValueAsString() const
	{
		if (m_enumOptions.empty() || m_value < 0 || m_value >= static_cast<int>(m_enumOptions.size()))
			return "";

		return m_enumOptions[m_value];
	}

	int EnumAttribute::getValueForString(const std::string& valueStr) const
	{
		auto it = std::find(m_enumOptions.begin(), m_enumOptions.end(), valueStr);

		if (it != m_enumOptions.end())
			return static_cast<int>(std::distance(m_enumOptions.begin(), it));

		return -1;
	}
}