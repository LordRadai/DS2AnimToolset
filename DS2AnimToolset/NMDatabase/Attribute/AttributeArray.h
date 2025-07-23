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
		virtual bool writeValueXML(int format) override;
		virtual void resize(uint32_t newSize) override { m_attributes.resize(newSize); }
		virtual bool writeStartArrayXML(int format) const override;
	};
}