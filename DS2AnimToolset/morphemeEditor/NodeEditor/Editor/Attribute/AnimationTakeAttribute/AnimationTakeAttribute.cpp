#include <stdexcept>

#include "NodeEditor/NodeEditor.h"
#include "AnimationTakeAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
    bool AnimationTakeAttribute::editorGUI()
    {
        bool readOnly = (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly) != 0;

        if (ImGui::TreeNodeEx(m_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
        {
            ImGui::BeginDisabled(readOnly);

            float labelWidth = 0.0f;
            const char* labels[] = { "File", "Take", "Sync Track" };
            for (const char* l : labels)
                labelWidth = ImMax(labelWidth, ImGui::CalcTextSize(l).x);

            float valueX = ImGui::GetCursorPosX() + labelWidth + ImGui::GetStyle().ItemInnerSpacing.x;

            auto Row = [&](const char* label, const char* value)
                {
                    ImGui::TextUnformatted(label);
                    ImGui::SameLine(valueX);
                    ImGui::Label(value);

                    if (!m_helpText.empty())
                        ImGui::SetItemTooltip(m_helpText.c_str());
                };

            Row("File", m_filename.c_str());
            Row("Take", m_takeName.c_str());
            Row("Sync Track", m_syncTrack.c_str());

            ImGui::EndDisabled();

            ImGui::TreePop();
        }

        return true;
    }

	void AnimationTakeAttribute::setValue(const std::vector<std::any>& values)
	{
		if (values.size() > 3 || values.size() < 2)
			throw std::runtime_error("AnimationTakeAttribute::setValue: Invalid number of values provided.");

		if (values[0].type() != typeid(std::string))
			throw std::runtime_error("AnimationTakeAttribute::setValue: Invalid type for filename.");

		m_filename = std::any_cast<std::string>(values[0]);

		if (values[1].type() != typeid(std::string))
			throw std::runtime_error("AnimationTakeAttribute::setValue: Invalid type for take name.");

		m_takeName = std::any_cast<std::string>(values[1]);

		if (values.size() == 3)
		{
			if (values[2].type() != typeid(std::string))
				throw std::runtime_error("AnimationTakeAttribute::setValue: Invalid type for sync track.");

			m_syncTrack = std::any_cast<std::string>(values[2]);
		}
	}
}
