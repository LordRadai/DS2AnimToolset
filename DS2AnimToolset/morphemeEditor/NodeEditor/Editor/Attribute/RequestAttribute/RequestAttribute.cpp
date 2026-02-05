#include "RCore.h"

#include "NodeEditor/NodeEditor.h"
#include "RequestAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool RequestAttribute::editorGUI()
	{
		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		if (m_request)
			ImGui::NamedLabel(m_name.c_str(), m_request->getName().c_str());
		else
			ImGui::NamedLabel(m_name.c_str(), "");

		if (!m_helpText.empty())
			ImGui::SetItemTooltip(m_helpText.c_str());

		ImGui::EndDisabled();

		return true;
	}

	void RequestAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() != 1)
			INVOKE_PANIC("RequestAttribute::setValue: Invalid number of values provided.");
		
		if (values[0].type() != typeid(Message*))
			INVOKE_PANIC("RequestAttribute::setValue: Invalid value type provided.");

		m_request = std::any_cast<Message*>(values[0]);
	}
}
