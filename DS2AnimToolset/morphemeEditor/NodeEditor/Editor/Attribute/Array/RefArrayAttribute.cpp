#include <stdexcept>

#include "NodeEditor/NodeEditor.h"
#include "RefArrayAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool RefArrayAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		for (auto& value : m_values)
		{
			char nameBuffer[32];
			sprintf_s(nameBuffer, "%s%d", m_name.c_str(), (int)&value);

			if (value)
				ImGui::NamedLabel(nameBuffer, value->getFullName().c_str());
			else
				ImGui::NamedLabel(nameBuffer, "");
		}

		ImGui::EndDisabled();

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
