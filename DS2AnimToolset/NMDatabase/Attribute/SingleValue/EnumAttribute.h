#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class EnumAttribute : public SingleValueAttribute
	{
		std::string m_value;
	public:
		EnumAttribute(Attribute* parent, std::string name, std::string value) :
			SingleValueAttribute(parent, name, "enum"),
			m_value(value)
		{
		};
		virtual ~EnumAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<EnumAttribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<EnumAttribute*>(other)->getValue(); }
		virtual EnumAttribute* asEnum() const override { return const_cast<EnumAttribute*>(this); }
		virtual bool isEnum() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;

		virtual bool getValueAsBool() const override { return !m_value.empty(); }
		virtual int getValueAsInt() const override { return stoi(m_value); }
		virtual float getValueAsFloat() const override { return stof(m_value); }
		virtual double getValueAsDouble() const override { return stod(m_value); }
		virtual std::string getValueAsString() const override { return m_value; }
		virtual NMP::Vector3 getValueAsVector3() const override { return NMP::Vector3Zero(); }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return NMP::Matrix34Zero(); }
		virtual NMP::Quat getValueAsQuaternion() const override { return NMP::QuatIdentity(); }
		virtual int getValueAsEnum() const override { return getValueAsInt(); }

		std::string getValue() const { return getValueAsString(); }
		void setValue(const std::string& value) { m_value = value; }
	};
}
