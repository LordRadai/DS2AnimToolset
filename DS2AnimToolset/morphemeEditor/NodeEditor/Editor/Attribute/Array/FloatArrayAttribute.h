#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class FloatArrayAttribute : public Attribute
	{
		std::vector<float> m_values;
	public:
		FloatArrayAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "flaotArray") {}

		virtual ~FloatArrayAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		const std::vector<float>& getValues() const { return m_values; }
	};
}
