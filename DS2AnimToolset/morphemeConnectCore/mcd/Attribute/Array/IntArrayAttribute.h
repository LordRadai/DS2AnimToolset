#pragma once
#include "../Attribute.h"

namespace mcd
{
	class IntArrayAttribute : public Attribute
	{
		std::unique_ptr<db::IntArrayAttribute> m_valueAttr = nullptr;

	public:
		IntArrayAttribute(db::Attribute* parent, std::string name)
			: Attribute(parent, "IntArrayAttribute", name),
			  m_valueAttr(new db::IntArrayAttribute(this, "Value"))
		{
			this->addAttribute(m_valueAttr.get());
		}

		virtual ~IntArrayAttribute() override {};
		virtual bool assignValue(Attribute* other) override;
		virtual bool isValueEqualTo(Attribute* attr) override;

		void addElement(int value) { m_valueAttr->add(value); }
		void setElement(int index, int value) { m_valueAttr->setElement(index, value); }
		int getElement(int index) const { return m_valueAttr->getElement(index); }
		uint32_t size() const { return m_valueAttr->size(); }
		bool empty() const { return m_valueAttr->empty(); }
	};
}
