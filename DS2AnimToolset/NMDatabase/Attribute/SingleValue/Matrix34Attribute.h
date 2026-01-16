#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class Matrix34Attribute : public SingleValueAttribute
	{
		NMP::Matrix34 m_value;

	public:
		Matrix34Attribute(CompositeAttribute* parent, std::string name, const NMP::Matrix34& value) :
			SingleValueAttribute(parent, name, "matrix34"),
			m_value(value)
		{};

		virtual ~Matrix34Attribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<Matrix34Attribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<Matrix34Attribute*>(other)->getValue(); }
		virtual Matrix34Attribute* asMatrix34() const override { return const_cast<Matrix34Attribute*>(this); }
		virtual bool isMatrix34() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;

		virtual bool getValueAsBool() const override { return false; }
		virtual int getValueAsEnum() const override { return 0; }
		virtual int getValueAsInt() const override { return 0; }
		virtual float getValueAsFloat() const override { return 0.f; }
		virtual double getValueAsDouble() const override { return 0.f; }
		virtual std::string getValueAsString() const override { return ""; }
		virtual NMP::Vector3 getValueAsVector3() const override { return NMP::Vector3Zero(); }
		virtual NMP::Quat getValueAsQuaternion() const override { return NMP::QuatIdentity(); }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return m_value; }

		NMP::Matrix34 getValue() const { return getValueAsMatrix34(); }
		void setValue(const NMP::Matrix34& value) { m_value = value; }
	};
}
