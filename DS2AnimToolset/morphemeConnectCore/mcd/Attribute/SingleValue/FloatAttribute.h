#pragma once
#include "../Attribute.h"

namespace mcd
{
	class FloatAttribute : public Attribute
	{
		db::FloatAttribute* m_valueAttr = nullptr;
	public:
		FloatAttribute(db::Node* parent, std::string name, float value);

		virtual ~FloatAttribute() override {};
		virtual bool assignValue(Attribute* other) override;
		virtual bool isValueEqualTo(Attribute* attr) override;

		float getValue() const { return m_valueAttr->getValue(); }
		void setValue(float value) { return m_valueAttr->setValue(value); }
	};
}