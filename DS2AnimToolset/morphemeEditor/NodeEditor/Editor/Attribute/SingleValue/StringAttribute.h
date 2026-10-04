#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class StringAttribute : public Attribute
	{
		std::string m_value;
	public:
		StringAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "string") {}

		virtual ~StringAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		const std::string& getValue() const { return m_value; }
	};
}
