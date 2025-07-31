#pragma once
#include "../Attribute.h"

namespace mcd
{
	class BoolArrayAttribute : public Attribute
	{
		std::unique_ptr<db::BoolArrayAttribute> m_valueAttr = nullptr;
	public:
		BoolArrayAttribute(db::Attribute* parent, std::string name);

		virtual ~BoolArrayAttribute() override {};
		virtual bool assignValue(Attribute* other) override;
		virtual bool isValueEqualTo(Attribute* attr) override;

		void addElement(bool value) { m_valueAttr->add(value); }
		void setElement(int index, bool value) { m_valueAttr->setElement(index, value); }
		bool getElement(int index) const { return m_valueAttr->getElement(index); }

		uint32_t size() const { return m_valueAttr->size(); }
		bool empty() const { return m_valueAttr->empty(); }
	};
}