#include "Condition.h"
#include "NodeEditor/Editor/Transition/Transition.h"
#include "imgui/imgui.h"

namespace NodeEditor
{
	Condition::Condition(Transition* owner, const std::string& typeName, const std::string& name) : Entity(owner->getOwnerEditor(), name),
		m_owner(owner), m_typeName(typeName)
	{
	}

	bool Condition::editorGUI()
	{
		if (ImGui::TreeNodeEx(m_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (Attribute* attrib : m_attributes)
				attrib->editorGUI();

			ImGui::TreePop();
		}

		return true;
	}

	const std::string Condition::getFullName() const
	{
		return m_owner->getFullName() + "." + m_name;
	}

	Attribute* Condition::getAttribute(size_t index) const
	{
		if (index < m_attributes.size())
			return m_attributes[index];

		return nullptr;
	}

	Attribute* Condition::getAttribute(const std::string& name) const
	{
		for (Attribute* attribute : m_attributes)
		{
			if (attribute->getName() == name)
				return attribute;
		}

		return nullptr;
	}
}
