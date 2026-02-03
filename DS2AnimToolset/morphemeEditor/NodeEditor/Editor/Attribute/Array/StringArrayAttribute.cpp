#include <stdexcept>

#include "NodeEditor/NodeEditor.h"
#include "StringArrayAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

#define TEMP_BUFFER_SIZE 1024

namespace NodeEditor
{
	bool StringArrayAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		for (size_t i = 0; i < m_values.size(); i++)
		{
			char nameBuffer[32];
			sprintf_s(nameBuffer, "%s%d", m_name.c_str(), (int)i);

			ImGui::NamedLabel(nameBuffer, m_values[i].c_str());

			if (!m_helpText.empty())
				ImGui::SetItemTooltip(m_helpText.c_str());
		}

		ImGui::EndDisabled();

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
