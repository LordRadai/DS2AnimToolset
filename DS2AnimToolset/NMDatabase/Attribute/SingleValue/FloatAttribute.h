#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class FloatAttribute : public SingleValueAttribute
	{
		float m_value;
	public:
		FloatAttribute(Attribute* parent, std::string name, float value) :
			SingleValueAttribute(parent, name, "float"),
			m_value(value)
		{
		};

		virtual ~FloatAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<FloatAttribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<FloatAttribute*>(other)->getValue(); }
		virtual FloatAttribute* asFloat() const override { return const_cast<FloatAttribute*>(this); }
		virtual bool isFloat() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;

		virtual bool getValueAsBool() const override { return getValue() != 0.0f; }
		virtual int getValueAsInt() const override { return static_cast<int>(getValue()); }
		virtual float getValueAsFloat() const override { return m_value; }
		virtual double getValueAsDouble() const override { return static_cast<double>(getValue()); }
		virtual std::string getValueAsString() const override { return std::to_string(getValue()); }
		virtual NMP::Vector3 getValueAsVector3() const override { return NMP::Vector3Zero(); }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return NMP::Matrix34Zero(); }
		virtual NMP::Quat getValueAsQuaternion() const override { return NMP::QuatIdentity(); }
		virtual int getValueAsEnum() const override { return static_cast<int>(getValue()); }

		float getValue() const { return getValueAsFloat(); }
		void setValue(float value) { m_value = value; }
	};
}
