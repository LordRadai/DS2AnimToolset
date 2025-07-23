#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class QuaternionAttribute : public SingleValueAttribute
	{
		NMP::Quat m_value;
	public:
		QuaternionAttribute(Attribute* parent, std::string name, std::string label, const NMP::Quat& value) :
			SingleValueAttribute(parent, name, label, "quaternion"),
			m_value(value)
		{
		};
		virtual ~QuaternionAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == (dynamic_cast<QuaternionAttribute*>(other)->getValue()); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<QuaternionAttribute*>(other)->getValue(); }
		virtual bool writeValueXML(int format) override;

		virtual NMP::Quat getValueAsQuaternion() const override { return m_value; }

		NMP::Quat getValue() const { return getValueAsQuaternion(); }
	};
}
