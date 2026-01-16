#pragma once
#include "../Attribute.h"

namespace mcd
{
	class BoolAttribute : public Attribute
	{
		std::unique_ptr<db::BoolAttribute> m_valueAttr = nullptr;
	public:
		BoolAttribute(db::CompositeAttribute* parent, std::string name, bool value);

		virtual ~BoolAttribute() override {};
		virtual bool assignValue(Attribute* other) override;
		virtual bool isValueEqualTo(Attribute* attr) override;

		bool getValue() const { return m_valueAttr->getValue(); }
		void setValue(bool value);
	};
}