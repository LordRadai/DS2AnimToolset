#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class RefAttribute : public Attribute
	{
		Entity* m_value;
	public:
		RefAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "ref"), m_value(nullptr) {}

		virtual ~RefAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		Entity* getValue() const { return m_value; }
	};
}
