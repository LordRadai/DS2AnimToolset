#pragma once
#include "SingleValueAttribute.inl"

namespace db
{
	class Vector3Attribute : public SingleValueAttribute
	{
		NMP::Vector3 m_value;
	public:
		Vector3Attribute(Attribute* parent, std::string name, std::string label, const NMP::Vector3& value) :
			SingleValueAttribute(parent, name, label, "vector3"),
			m_value(value)
		{};

		virtual ~Vector3Attribute() override {};
		virtual bool compare(Attribute* other) override { return m_value == (dynamic_cast<Vector3Attribute*>(other)->getValue()); }
		virtual void assign(Attribute* other) override { m_value = dynamic_cast<Vector3Attribute*>(other)->getValue(); }
		virtual bool writeValueXML(int format) override;

		virtual std::string getValueAsString() const;
		virtual NMP::Vector3 getValueAsVector3() const override { return m_value; }
		NMP::Vector3 getValue() const { return getValueAsVector3(); }
	};
}
