#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class StringAttribute : public SingleValueAttribute
	{
		std::string m_value;
	public:
		StringAttribute(Attribute* parent, std::string name, std::string value) :
			SingleValueAttribute(parent, name, "string"),
			m_value(value)
		{};

		virtual ~StringAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<StringAttribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<StringAttribute*>(other)->getValue(); }
		virtual StringAttribute* asString() const override { return const_cast<StringAttribute*>(this); }
		virtual bool isString() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;

		virtual bool getValueAsBool() const override { return !m_value.empty(); }
		virtual int getValueAsEnum() const override { return getValueAsInt(); }
		virtual int getValueAsInt() const override { return std::stoi(getValue()); }
		virtual float getValueAsFloat() const override { return std::stof(getValue()); }
		virtual double getValueAsDouble() const override { return std::stod(getValue()); }
		virtual std::string getValueAsString() const override { return m_value; }
		virtual NMP::Vector3 getValueAsVector3() const override { return NMP::Vector3Zero(); }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return NMP::Matrix34Zero(); }
		virtual NMP::Quat getValueAsQuaternion() const override { return NMP::QuatIdentity(); }

		const std::string& getValue() const { return getValueAsString(); }
		void setValue(const std::string& value) { m_value = value; }
	};
}
