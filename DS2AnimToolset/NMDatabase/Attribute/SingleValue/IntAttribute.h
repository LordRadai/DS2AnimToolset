#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class IntAttribute : public SingleValueAttribute
	{
		int m_value;
	public:
		IntAttribute(Attribute* parent, std::string name, int value) :
			SingleValueAttribute(parent, name, "int"),
			m_value(value)
		{
		};

		virtual ~IntAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<IntAttribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<IntAttribute*>(other)->getValue(); }
		virtual IntAttribute* asInt() const override { return const_cast<IntAttribute*>(this); }
		virtual bool isInt() const override { return true; }
		virtual bool writeValueXML(int format) override;

		virtual bool getValueAsBool() const override { return getValue() != 0; }
		virtual int getValueAsInt() const override { return m_value; }
		virtual float getValueAsFloat() const override { return static_cast<float>(getValue()); }
		virtual double getValueAsDouble() const override { return static_cast<double>(getValue()); }
		virtual std::string getValueAsString() const override { return std::to_string(getValue()); }
		virtual NMP::Vector3 getValueAsVector3() const override { return NMP::Vector3Zero(); }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return NMP::Matrix34Zero(); }
		virtual NMP::Quat getValueAsQuaternion() const override { return NMP::QuatIdentity(); }
		virtual int getValueAsEnum() const override { return getValue(); }

		int getValue() const { return getValueAsInt(); }
		void setValue(int value) { m_value = value; }
	};
}
