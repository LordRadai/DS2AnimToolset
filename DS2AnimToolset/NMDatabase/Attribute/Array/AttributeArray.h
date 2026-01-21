#pragma once
#include "../AttributeContainer/AttributeContainer.h"

namespace db
{
	class AttributeArray : public AttributeContainer
	{
	public:
		AttributeArray(CompositeAttribute* parent, std::string name) :
			AttributeContainer(parent, name, "attributeArray") { };

		virtual ~AttributeArray() override {};
		virtual void assign(db::Attribute* other) override;
		virtual AttributeArray* asAttributeArray() const override { return const_cast<AttributeArray*>(this); }
		virtual bool isAttributeArray() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;
		virtual void resize(uint32_t newSize) override { m_attributes.resize(newSize); }
		virtual bool writeStartArrayXML(int format, SaverXML* saver) const override;

		Attribute* add();
		Attribute* insert(int idx);

		std::string getElemType() const;
	};
}