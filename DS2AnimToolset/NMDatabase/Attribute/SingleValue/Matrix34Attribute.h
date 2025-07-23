#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class Matrix34Attribute : public SingleValueAttribute
	{
		NMP::Matrix34 m_value;

	public:
		Matrix34Attribute(Attribute* parent, std::string name, const NMP::Matrix34& value) :
			SingleValueAttribute(parent, name, "matrix34"),
			m_value(value)
		{};

		virtual ~Matrix34Attribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<Matrix34Attribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<Matrix34Attribute*>(other)->getValue(); }
		virtual bool writeValueXML(int format) override;

		virtual NMP::Matrix34 getValueAsMatrix34() const override { return m_value; }

		NMP::Matrix34 getValue() const { return getValueAsMatrix34(); }
	};
}
