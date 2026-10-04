#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class BoolArrayAttribute : public Attribute
	{
		std::vector<bool> m_values;
	public:
		BoolArrayAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "boolArray") {}

		virtual ~BoolArrayAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		const std::vector<bool>& getValues() const { return m_values; }
	};
}
