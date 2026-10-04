#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class FloatAttribute : public Attribute
	{
		float m_value;
	public:
		FloatAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "float") {}

		virtual ~FloatAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		float getValue() const { return m_value; }
	};
}
