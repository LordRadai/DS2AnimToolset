#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class RefAttribute : public Attribute
	{
		Entity* m_value;
		std::string m_refKind;
		bool m_isWeakRef;
	public:
		RefAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "ref"), m_value(nullptr), m_refKind(""), m_isWeakRef(false) {}

		virtual ~RefAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		Entity* getValue() const { return m_value; }

		const std::string& getRefKind() const { return m_refKind; }
		void setRefKind(const std::string& kind) { m_refKind = kind; }

		bool isWeakRef() const { return m_isWeakRef; }
		void setWeakRef(bool isWeak) { m_isWeakRef = isWeak; }
	};
}
