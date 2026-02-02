#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class StringArrayAttribute : public Attribute
	{
		std::vector<std::string> m_values;
	public:
		StringArrayAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "stringArray") {}

		virtual ~StringArrayAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		const std::vector<std::string>& getValues() const { return m_values; }
	}
}
