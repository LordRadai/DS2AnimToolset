#pragma once
#include "../Attribute.h"
#include "NMDBExtensions/TypedAttributeArray.inl"

namespace mcd
{
	class IntArrayAttribute : public Attribute
	{
		std::unique_ptr<db::TypedAttributeArray<db::IntAttribute>> m_valueAttr = nullptr;
	public:
		IntArrayAttribute(db::Attribute* parent, std::string name);

		virtual ~IntArrayAttribute() override {};
		virtual bool assignValue(Attribute* other) override;
		virtual bool isValueEqualTo(Attribute* attr) override;

		void removeElement(int index);
		void addElement(int value);
		void setElement(int index, int value) { m_valueAttr->getAttribute(index)->asInt()->setValue(value); }
		int getElement(int index) const { return m_valueAttr->getAttribute(index)->asInt()->getValue(); }

		uint32_t size() const { return m_valueAttr->size(); }
		bool empty() const { return m_valueAttr->empty(); }
	};
}
