#include <stdexcept>

#include "NodeEditor/NodeEditor.h"
#include "AnimationTakeAttribute.h"
#include "imgui/imgui.h"
#include "imgui_custom/imgui_custom_widget.h"

namespace NodeEditor
{
	bool AnimationTakeAttribute::editorGUI()
	{
		ImGui::TextUnformatted(m_name.c_str());

		bool readOnly = false;
		if (m_ownerEditor->getFlags() & NodeEditorFlags_ReadOnly)
			readOnly = true;

		ImGui::BeginDisabled(readOnly);

		ImGui::NamedLabel("File", m_filename.c_str());
		ImGui::NamedLabel("Take", m_takeName.c_str());
		ImGui::NamedLabel("Sync Track", m_syncTrack.c_str());

		ImGui::EndDisabled();

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
