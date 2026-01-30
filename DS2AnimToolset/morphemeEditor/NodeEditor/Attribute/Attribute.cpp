#include "Attribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	void Attribute::editorGUI()
	{
		ImGui::TreeNode(m_name.c_str());

		ImGui::TreePop();
	}
}
