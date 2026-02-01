#include "Condition.h"
#include "NodeEditor/Editor/Transition/Transition.h"
#include "imgui/imgui.h"

namespace NodeEditor
{
	Condition::Condition(Transition* owner, const std::string& typeName, const std::string& name) : Entity(owner->getOwnerEditor(), name),
		m_owner(owner), m_typeName(typeName)
	{
	}

	void Condition::editorGUI()
	{
		ImGui::TreeNode(m_name.c_str());

		for (Attribute* attrib : m_attributes)
			attrib->editorGUI();

		ImGui::TreePop();
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
