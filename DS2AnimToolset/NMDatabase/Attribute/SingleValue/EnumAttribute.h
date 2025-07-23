#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class EnumAttribute : public SingleValueAttribute
	{
		int m_value;
	public:
		EnumAttribute(Attribute* parent, std::string name, std::string label, int value) :
			SingleValueAttribute(parent, name, label, "enum"),
			m_value(value)
		{
		};
		virtual ~EnumAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<EnumAttribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<EnumAttribute*>(other)->getValue(); }
		virtual bool writeValueXML(int format) override;

		virtual int getValueAsEnum() const override { return m_value; }

		int getValue() const { return getValueAsEnum(); }
	};
}
