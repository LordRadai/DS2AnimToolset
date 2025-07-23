#pragma once
#include "Attribute/Attribute.h"

namespace db
{
	class ArrayAttribute : public Attribute
	{
	public:
		ArrayAttribute(Attribute* parent, std::string name, std::string label, std::string type) :
			Attribute(parent, name, label, type) {};

		virtual ~ArrayAttribute() override {};
		virtual uint32_t size() const = 0;
		virtual bool empty() const = 0;
		virtual void clearArray() = 0;
		virtual void removeAt(int idx) = 0;
		virtual void resize(uint32_t newSize) = 0;
		virtual bool writeStartArrayXML(int format) const = 0;
	};
}
