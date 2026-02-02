#include <stdexcept>

#include "AnimationTakeAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool AnimationTakeAttribute::editorGUI()
	{
		ImGui::TextUnformatted(m_name.c_str());

		ImGui::TextUnformatted("File");
		ImGui::SameLine();
		ImGui::Label(m_filename.c_str());

		ImGui::TextUnformatted("Take");
		ImGui::SameLine();
		ImGui::Label(m_takeName.c_str());

		ImGui::TextUnformatted("Sync Track");
		ImGui::SameLine();
		ImGui::Label(m_syncTrack.c_str());
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
