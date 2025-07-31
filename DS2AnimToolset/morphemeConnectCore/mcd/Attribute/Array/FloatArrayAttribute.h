#pragma once
#include "../Attribute.h"

namespace mcd
{
	class FloatArrayAttribute : public Attribute
	{
		std::unique_ptr<db::FloatArrayAttribute> m_valueAttr = nullptr;

	public:
		FloatArrayAttribute(db::Attribute* parent, std::string name);

		virtual ~FloatArrayAttribute() override {};
		virtual bool assignValue(Attribute* other) override;
		virtual bool isValueEqualTo(Attribute* attr) override;

		void addElement(float value) { m_valueAttr->add(value); }
		void setElement(int index, float value) { m_valueAttr->setElement(index, value); }
		float getElement(int index) const { return m_valueAttr->getElement(index); }
		uint32_t size() const { return m_valueAttr->size(); }
		bool empty() const { return m_valueAttr->empty(); }
	};
}
