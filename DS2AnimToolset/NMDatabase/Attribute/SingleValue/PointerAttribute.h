#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class PointerAttribute : public SingleValueAttribute
	{
		std::string m_value;
	public:
		PointerAttribute(Attribute* parent, std::string name, Attribute* to) :
			SingleValueAttribute(parent, name, "pointer"),
			m_value(to->toDatabasePath())
		{};

		virtual ~PointerAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<PointerAttribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<PointerAttribute*>(other)->getValue(); }
		virtual bool writeValueXML(int format) override;

		virtual std::string getValueAsString() const override { return m_value; }

		std::string getValue() const { return getValueAsString(); }
	};
}
