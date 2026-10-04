#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class RefArrayAttribute : public Attribute
	{
		std::vector<Entity*> m_values;
		std::string m_refKind;
		bool m_isWeakRef;
	public:
		RefArrayAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "refArray") {}

		virtual ~RefArrayAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		const std::vector<Entity*>& getValues() const { return m_values; }

		void setRefKind(const std::string& kind) { m_refKind = kind; }
		const std::string& getRefKind() const { return m_refKind; }

		bool isWeakRef() const { return m_isWeakRef; }
		void setWeakRef(bool isWeak) { m_isWeakRef = isWeak; }
	};
}