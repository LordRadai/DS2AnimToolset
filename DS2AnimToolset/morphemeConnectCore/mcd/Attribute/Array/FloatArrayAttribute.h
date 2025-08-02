#pragma once
#include "../Attribute.h"
#include "NMDBExtensions/TypedAttributeArray.inl"

namespace mcd
{
	class FloatArrayAttribute : public Attribute
	{
		std::unique_ptr<db::TypedAttributeArray<db::FloatAttribute>> m_valueAttr = nullptr;
	public:
		FloatArrayAttribute(db::Attribute* parent, std::string name);

		virtual ~FloatArrayAttribute() override {};
		virtual bool assignValue(Attribute* other) override;
		virtual bool isValueEqualTo(Attribute* attr) override;

		void removeElement(int index);
		void addElement(float value);
		void setElement(int index, float value) { m_valueAttr->getAttribute(index)->asFloat()->setValue(value); }
		float getElement(int index) const { return m_valueAttr->getAttribute(index)->asFloat()->getValue(); }

		uint32_t size() const { return m_valueAttr->size(); }
		bool empty() const { return m_valueAttr->empty(); }
	};
}
