#pragma once
#include "../Attribute.h"

namespace mcd
{
	class IntAttribute : public Attribute
	{
		std::unique_ptr<db::IntAttribute> m_valueAttr = nullptr;

	public:
		IntAttribute(db::Attribute* parent, std::string name, int value);

		virtual ~IntAttribute() override {};
		virtual bool assignValue(Attribute* other) override;
		virtual bool isValueEqualTo(Attribute* attr) override;

		int getValue() const { return m_valueAttr->getValue(); }
		void setValue(int value) { return m_valueAttr->setValue(value); }
	};
}
