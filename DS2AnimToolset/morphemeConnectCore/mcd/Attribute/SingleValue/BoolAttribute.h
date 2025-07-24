#pragma once
#include "../Attribute.h"

namespace mcd
{
	class BoolAttribute : public Attribute
	{
		db::BoolAttribute* m_valueAttr = nullptr;
	public:
		BoolAttribute(db::Node* parent, std::string name, bool value);

		virtual ~BoolAttribute() override {};
		virtual bool assignValue(Attribute* other) override;

		bool getValue() const { return m_valueAttr->getValue(); }
		void setValue(bool value) { return m_valueAttr->setValue(value); }
	};
}