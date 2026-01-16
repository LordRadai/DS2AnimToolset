#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class BoolAttribute : public SingleValueAttribute
	{
		bool m_value;
	public:
		BoolAttribute(CompositeAttribute* parent, std::string name, bool value) :
			SingleValueAttribute(parent, name, "bool"),
			m_value(value)
		{};

		virtual ~BoolAttribute() override {};

		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<BoolAttribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<BoolAttribute*>(other)->getValue(); }
		virtual BoolAttribute* asBool() const override { return const_cast<BoolAttribute*>(this); }
		virtual bool isBool() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;
		virtual bool getValueAsBool() const override { return m_value; }
		virtual int getValueAsEnum() const override { return static_cast<int>(getValueAsBool()); }
		virtual int getValueAsInt() const override { return static_cast<int>(getValueAsBool()); }
		virtual float getValueAsFloat() const override { return static_cast<float>(getValueAsBool()); }
		virtual double getValueAsDouble() const override { return static_cast<double>(getValueAsBool()); }
		virtual std::string getValueAsString() const override { return m_value ? "true" : "false"; }
		virtual NMP::Vector3 getValueAsVector3() const override { return NMP::Vector3Zero(); }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return NMP::Matrix34Zero(); }
		virtual NMP::Quat getValueAsQuaternion() const override { return NMP::QuatIdentity(); }

		bool getValue() const { return getValueAsBool(); }
		void setValue(bool value) { m_value = value; }
	};
}
