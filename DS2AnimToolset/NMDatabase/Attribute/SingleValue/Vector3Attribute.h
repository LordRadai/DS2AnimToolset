#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class Vector3Attribute : public SingleValueAttribute
	{
		NMP::Vector3 m_value;
	public:
		Vector3Attribute(CompositeAttribute* parent, std::string name, const NMP::Vector3& value) :
			SingleValueAttribute(parent, name, "vector3"),
			m_value(value)
		{};

		virtual ~Vector3Attribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == (dynamic_cast<Vector3Attribute*>(other)->getValue()); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<Vector3Attribute*>(other)->getValue(); }
		virtual Vector3Attribute* asVector3() const override { return const_cast<Vector3Attribute*>(this); }
		virtual bool isVector3() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;

		virtual bool getValueAsBool() const override { return false; }
		virtual int getValueAsEnum() const override { return 0; }
		virtual int getValueAsInt() const override { return 0; }
		virtual float getValueAsFloat() const override { return 0.0f; }
		virtual double getValueAsDouble() const override { return 0.0; }
		virtual std::string getValueAsString() const;
		virtual NMP::Vector3 getValueAsVector3() const override { return m_value; }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return NMP::Matrix34(m_value); }
		virtual NMP::Quat getValueAsQuaternion() const override { return NMP::Quat(m_value.x, m_value.y, m_value.z, 0.f); }

		NMP::Vector3 getValue() const { return getValueAsVector3(); }
		void setValue(const NMP::Vector3& value) { m_value = value; }
	};
}
