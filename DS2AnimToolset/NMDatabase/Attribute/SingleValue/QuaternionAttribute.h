#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class QuaternionAttribute : public SingleValueAttribute
	{
		NMP::Quat m_value;
	public:
		QuaternionAttribute(Attribute* parent, std::string name, const NMP::Quat& value) :
			SingleValueAttribute(parent, name, "quaternion"),
			m_value(value)
		{
		};
		virtual ~QuaternionAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == (dynamic_cast<QuaternionAttribute*>(other)->getValue()); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<QuaternionAttribute*>(other)->getValue(); }
		virtual QuaternionAttribute* asQuaternion() const override { return const_cast<QuaternionAttribute*>(this); }
		virtual bool isQuaternion() const override { return true; }
		virtual bool writeValueXML(int format) override;

		virtual bool getValueAsBool() const override { return false; }
		virtual int getValueAsEnum() const override { return 0; }
		virtual int getValueAsInt() const override { return 0; }
		virtual float getValueAsFloat() const override { return 0.0f; }
		virtual double getValueAsDouble() const override { return 0.0; }
		virtual std::string getValueAsString() const override;
		virtual NMP::Vector3 getValueAsVector3() const override { return NMP::Vector3Zero(); }
		virtual NMP::Quat getValueAsQuaternion() const override { return m_value; }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return NMP::Matrix34Zero(); }

		NMP::Quat getValue() const { return getValueAsQuaternion(); }
	};
}
