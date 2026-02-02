#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class IntAttribute : public Attribute
	{
		int m_value;
	public:
		IntAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "int") {}

		virtual ~IntAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		int getValue() const { return m_value; }
	};
}
