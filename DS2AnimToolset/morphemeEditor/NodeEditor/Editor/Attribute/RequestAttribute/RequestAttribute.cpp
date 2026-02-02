#include <stdexcept>

#include "RequestAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool RequestAttribute::editorGUI()
	{
		ImGui::TextUnformatted(m_name.c_str());

		if (m_request)
			ImGui::Label(m_request->getName().c_str());
		else
			ImGui::Label("<No Request Assigned>");
	}

	void RequestAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() != 1)
			throw std::runtime_error("RequestAttribute::setValue: Invalid number of values provided.");
		
		if (values[0].type() != typeid(Request*))
			throw std::runtime_error("RequestAttribute::setValue: Invalid value type provided.");

		m_request = std::any_cast<Request*>(values[0]);
	}
}
