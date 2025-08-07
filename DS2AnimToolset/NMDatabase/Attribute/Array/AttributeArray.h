#pragma once
#include "../AttributeContainer/AttributeContainer.h"

namespace db
{
	class AttributeArray : public AttributeContainer
	{
	public:
		AttributeArray(Attribute* parent, std::string name) :
			AttributeContainer(parent, name, "attributeArray") { };

		virtual ~AttributeArray() override {};
		virtual void assign(db::Attribute* other) override
		{
			AttributeArray* otherArray = other->asAttributeArray();

			if (otherArray == nullptr)
				throw std::runtime_error("AttributeArray::assign() failed - Other attribute is not an AttributeArray");

			m_attributes.clear();
			m_attributes.reserve(otherArray->size());
			for (size_t i = 0; i < otherArray->size(); ++i)
			{
				Attribute* attr = otherArray->getAttribute(i);
				if (attr != nullptr)
					m_attributes.push_back(attr);
			}
		}

		virtual AttributeArray* asAttributeArray() const override { return const_cast<AttributeArray*>(this); }
		virtual bool isAttributeArray() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;
		virtual void resize(uint32_t newSize) override { m_attributes.resize(newSize); }
		virtual bool writeStartArrayXML(int format, SaverXML* saver) const override;

		void add(Attribute* attribute)
		{
			if (attribute == nullptr)
				throw std::invalid_argument("AttributeArray::add() - Attribute cannot be null");

			m_attributes.push_back(attribute);
			attribute->setParentAttribute(this);
		}

		void insert(Attribute* attribute, int idx) { CompositeAttribute::insertAttribute(idx, attribute); }

		std::string getElemType() const 
		{
			if (m_attributes.empty())
				return "unknown";

			return m_attributes.front()->getType();
		}
	};
}