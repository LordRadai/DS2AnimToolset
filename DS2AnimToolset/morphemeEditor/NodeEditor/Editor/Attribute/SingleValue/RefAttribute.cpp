#include <stdexcept>

#include "NodeEditor/NodeEditor.h"
#include "RefAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool RefAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		if (m_value)
			ImGui::NamedLabel(m_name.c_str(), m_value->getFullName().c_str());
		else 
			ImGui::NamedLabel(m_name.c_str(), "");

		ImGui::EndDisabled();

		return true;
	}

	void RefAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() > 3)
			throw std::runtime_error("RefAttribute::setValue: Invalid number of values provided.");
		
		if (values[0].type() != typeid(Entity*))
			throw std::runtime_error("RefAttribute::setValue: Invalid value type provided.");

		m_value = std::any_cast<Entity*>(values[0]);

		if (values.size() >= 2)
		{
			if (values[1].type() != typeid(std::string))
				throw std::runtime_error("RefAttribute::setValue: Invalid ref kind type provided.");

			m_refKind = std::any_cast<std::string>(values[1]);
		}

		if (values.size() == 3)
		{
			if (values[2].type() != typeid(bool))
				throw std::runtime_error("RefAttribute::setValue: Invalid isWeakRef type provided.");

			m_isWeakRef = std::any_cast<bool>(values[2]);
		}
	}
}