#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class IntArrayAttribute : public Attribute
	{
		std::vector<int> m_values;
	public:
		IntArrayAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "intArray") {}

		virtual ~IntArrayAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		const std::vector<int>& getValues() const { return m_values; }
	};
}
