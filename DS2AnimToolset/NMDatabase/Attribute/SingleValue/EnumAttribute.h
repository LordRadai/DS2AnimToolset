#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class EnumAttribute : public SingleValueAttribute
	{
		int m_value;
	public:
		EnumAttribute(Attribute* parent, std::string name, int value) :
			SingleValueAttribute(parent, name, "enum"),
			m_value(value)
		{
		};
		virtual ~EnumAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<EnumAttribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<EnumAttribute*>(other)->getValue(); }
		virtual bool writeValueXML(int format) override;

		virtual bool getValueAsBool() const override { return m_value != 0; }
		virtual int getValueAsInt() const override { return m_value; }
		virtual float getValueAsFloat() const override { return static_cast<float>(m_value); }
		virtual double getValueAsDouble() const override { return static_cast<double>(m_value); }
		virtual std::string getValueAsString() const override { return std::to_string(m_value); }
		virtual NMP::Vector3 getValueAsVector3() const override { return NMP::Vector3Zero(); }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return NMP::Matrix34Zero(); }
		virtual NMP::Quat getValueAsQuaternion() const override { return NMP::QuatIdentity(); }
		virtual int getValueAsEnum() const override { return m_value; }

		int getValue() const { return getValueAsEnum(); }
	};
}
