#pragma once
#include "../Attribute.h"
#include "NMDBExtensions/TypedAttributeArray.inl"

namespace mcd
{
	class BoolArrayAttribute : public Attribute
	{
		std::unique_ptr<db::TypedAttributeArray<db::BoolAttribute>> m_valueAttr = nullptr;
	public:
		BoolArrayAttribute(db::Attribute* parent, std::string name);

		virtual ~BoolArrayAttribute() override {};
		virtual bool assignValue(Attribute* other) override;
		virtual bool isValueEqualTo(Attribute* attr) override;

		void removeElement(int index);
		void addElement(bool value);
		void setElement(int index, bool value) { m_valueAttr->getAttribute(index)->asBool()->setValue(value); }
		bool getElement(int index) const { return m_valueAttr->getAttribute(index)->asBool()->getValue(); }

		uint32_t size() const { return m_valueAttr->size(); }
		bool empty() const { return m_valueAttr->empty(); }
	};
}