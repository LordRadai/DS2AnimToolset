#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class BoolAttribute : public Attribute
	{
		bool m_value;
	public:
		BoolAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "bool"), m_value(false) {}
		virtual ~BoolAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		bool getValue() const { return m_value; }
	};
}
