#pragma once
#include "AttributeContainer.h"

namespace db
{
	class AttributeArray : public AttributeContainer
	{
	public:
		AttributeArray(Attribute* parent, std::string name, std::string type) :
			AttributeContainer(parent, name, type) {};

		virtual ~AttributeArray() override {};
		virtual AttributeArray* asAttributeArray() const override { return const_cast<AttributeArray*>(this); }
		virtual bool isAttributeArray() const override { return true; }
		virtual bool writeValueXML(int format) override;
		virtual void resize(uint32_t newSize) override { m_attributes.resize(newSize); }
		virtual bool writeStartArrayXML(int format) const override;
	};
}