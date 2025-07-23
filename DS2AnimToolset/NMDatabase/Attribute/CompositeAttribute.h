#pragma once
#include "Array/ArrayAttribute.h"

namespace db
{
	class CompositeAttribute : public ArrayAttribute
	{
		int getAttributeIndex(Attribute* attribute);

	protected:
		std::vector<Attribute*> m_attributes;

	public:
		CompositeAttribute(Attribute* parent, std::string name, std::string label, std::string type) : 
			ArrayAttribute(parent, name, label, type) {};

		virtual ~CompositeAttribute() override {};
		virtual uint32_t getAttributeCount() const override { return m_attributes.size(); }
		virtual bool empty() const override { return m_attributes.empty(); }
		virtual void removeAt(int idx) override;
		virtual bool writeStartArrayXML(int format) const override { return false; }

		void deleteChildren();
		void insertAttribute(int idx, Attribute* attribute);
		void insertAttributeUnmanaged(int idx, Attribute* attribute);
		void removeAttribute(Attribute* attribute);
		void removeAttributeUnmanaged(Attribute* attribute);
		Attribute* back() const { return m_attributes.back(); }
		Attribute* front() const { return m_attributes.front(); }
		Attribute* getAttribute(int idx) const;
		Attribute* findAttribute(std::string name) const;

		Attribute* operator[] (uint32_t idx) const
		{
			if (idx < 0 || idx >= static_cast<int>(m_attributes.size()))
				throw std::out_of_range("CompositeAttribute::operator[] - Index out of range");

			return m_attributes[idx];
		}
	};
}