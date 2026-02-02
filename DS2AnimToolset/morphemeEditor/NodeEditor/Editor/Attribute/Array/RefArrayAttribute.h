#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class RefArrayAttribute : public Attribute
	{
		std::vector<Entity*> m_values;
	public:
		RefArrayAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "refArray") {}

		virtual ~RefArrayAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		const std::vector<Entity*>& getValues() const { return m_values; }
	};
}