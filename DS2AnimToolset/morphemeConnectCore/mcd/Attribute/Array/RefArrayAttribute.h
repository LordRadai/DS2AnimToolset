#pragma once
#include "../Attribute.h"
#include "NMDBExtensions/TypedAttributeArray.inl"
#include "mcd/AttributePinNodeBase/AttributePinNodeBase.h"

namespace mcd
{
	class RefArrayAttribute : public Attribute
	{
		std::unique_ptr<db::TypedAttributeArray<AttributePinNodeBase>> m_valueAttr = nullptr;
	public:
		RefArrayAttribute(db::CompositeAttribute* parent, std::string name);

		virtual ~RefArrayAttribute() override {};
		virtual bool assignValue(Attribute* other) override;
		virtual bool isValueEqualTo(Attribute* attr) override;

		void removeElement(int index);
		void addElement(AttributePinNodeBase* value);
		void setElement(int index, AttributePinNodeBase* value);
		AttributePinNodeBase* getElement(int index);

		uint32_t size() const { return m_valueAttr->size(); }
		bool empty() const { return m_valueAttr->empty(); }
	};
}
