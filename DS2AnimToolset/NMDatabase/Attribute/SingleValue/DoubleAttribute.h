#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class DoubleAttribute : public SingleValueAttribute
	{
		double m_value;
	public:
		DoubleAttribute(Attribute* parent, std::string name, double value) :
			SingleValueAttribute(parent, name, "double"),
			m_value(value)
		{
		};
		virtual ~DoubleAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<DoubleAttribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<DoubleAttribute*>(other)->getValue(); }
		virtual DoubleAttribute* asDouble() const override { return const_cast<DoubleAttribute*>(this); }
		virtual bool isDouble() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;
		virtual bool getValueAsBool() const override { return m_value != 0.0; }
		virtual int getValueAsEnum() const override { return static_cast<int>(getValue()); }
		virtual int getValueAsInt() const override { return static_cast<int>(getValue()); }
		virtual float getValueAsFloat() const override { return static_cast<float>(getValue()); }
		virtual double getValueAsDouble() const override { return m_value; }
		virtual std::string getValueAsString() const override { return std::to_string(getValue()); }
		virtual NMP::Vector3 getValueAsVector3() const override { return NMP::Vector3Zero(); }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return NMP::Matrix34Zero(); }
		virtual NMP::Quat getValueAsQuaternion() const override { return NMP::QuatIdentity(); }

		double getValue() const { return getValueAsDouble(); }
		void setValue(double value) { m_value = value; }
	};
}
