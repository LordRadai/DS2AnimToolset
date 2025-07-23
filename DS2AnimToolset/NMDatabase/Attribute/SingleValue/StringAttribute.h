#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class StringAttribute : public SingleValueAttribute
	{
		std::string m_value;
	public:
		StringAttribute(Attribute* parent, std::string name, std::string label, std::string value) :
			SingleValueAttribute(parent, name, label, "string"),
			m_value(value)
		{};

		virtual ~StringAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<StringAttribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<StringAttribute*>(other)->getValue(); }
		virtual bool writeValueXML(int format) override;

		virtual int getValueAsEnum() const override { return getValueAsInt(); }
		virtual int getValueAsInt() const override { return std::stoi(getValue()); }
		virtual float getValueAsFloat() const override { return std::stof(getValue()); }
		virtual double getValueAsDouble() const override { return std::stod(getValue()); }
		virtual std::string getValueAsString() const override { return m_value; }

		const std::string& getValue() const { return getValueAsString(); }
	};
}
