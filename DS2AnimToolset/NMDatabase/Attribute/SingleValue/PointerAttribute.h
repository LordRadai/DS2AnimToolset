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

		virtual bool getValueAsBool() const override { return !m_value.empty(); }
		virtual int getValueAsEnum() const override { return static_cast<int>(!m_value.empty()); }
		virtual int getValueAsInt() const override { return static_cast<int>(!m_value.empty()); }
		virtual float getValueAsFloat() const override { return static_cast<float>(!m_value.empty()); }
		virtual double getValueAsDouble() const override { return static_cast<double>(!m_value.empty()); }
		virtual NMP::Vector3 getValueAsVector3() const override { return NMP::Vector3Zero(); }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return NMP::Matrix34Zero(); }
		virtual NMP::Quat getValueAsQuaternion() const override { return NMP::QuatIdentity(); }
		virtual std::string getValueAsString() const override { return m_value; }

		std::string getValue() const { return getValueAsString(); }
	};
}
