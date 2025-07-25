#pragma once
#include "../Attribute.h"

namespace mcd
{
	class StringAttribute : public Attribute
	{
		db::StringAttribute* m_valueAttr = nullptr;

	public:
		StringAttribute(db::Node* parent, std::string name, const std::string& value);

		virtual ~StringAttribute() override {};
		virtual bool assignValue(Attribute* other) override;
		virtual bool isValueEqualTo(Attribute* attr) override;

		std::string getValue() const { return m_valueAttr->getValue(); }
		void setValue(const std::string& value) { return m_valueAttr->setValue(value); }
	};
}
