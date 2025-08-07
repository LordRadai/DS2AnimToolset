#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class PointerAttribute : public SingleValueAttribute
	{
		std::string m_value;
	public:
		PointerAttribute(Attribute* parent, std::string name, Attribute* to) :
			SingleValueAttribute(parent, name, "pointer")
		{
			if (to)
				m_value = to->toDatabasePath();
		};

		virtual ~PointerAttribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == dynamic_cast<PointerAttribute*>(other)->getValue(); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<PointerAttribute*>(other)->getValue(); }
		virtual PointerAttribute* asPointer() const override { return const_cast<PointerAttribute*>(this); }
		virtual bool isPointer() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;

		virtual bool getValueAsBool() const override { return m_value.empty(); }
		virtual int getValueAsEnum() const override { return 0; }
		virtual int getValueAsInt() const override { return 0; }
		virtual float getValueAsFloat() const override { return 0.f; }
		virtual double getValueAsDouble() const override { return 0.0; }
		virtual NMP::Vector3 getValueAsVector3() const override { return NMP::Vector3Zero(); }
		virtual NMP::Matrix34 getValueAsMatrix34() const override { return NMP::Matrix34Zero(); }
		virtual NMP::Quat getValueAsQuaternion() const override { return NMP::QuatIdentity(); }
		virtual std::string getValueAsString() const override { return m_value; }

		std::string getValue() const { return getValueAsString(); }
		void setValue(db::Attribute* value) { m_value = value->toDatabasePath(); }
	};
}
