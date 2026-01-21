#pragma once
#include "AttributeArray.h"
#include "../SingleValue/PointerAttribute.h"

namespace db
{
	class PointerArrayAttribute : public AttributeArray
	{
	public:
		PointerArrayAttribute(CompositeAttribute* parent, const std::string& name) :
			AttributeArray(parent, name)
		{};

		virtual ~PointerArrayAttribute() override {}
		virtual PointerArrayAttribute* asPointerArray() const override { return const_cast<PointerArrayAttribute*>(this); }

		PointerAttribute* add(Attribute* to);
		PointerAttribute* insert(Attribute* to, int idx);
		Attribute* find(const std::string& name) const;

		void setElement(Attribute* to, int index);
		void setValue(std::vector<Attribute*> values);
	};
}
