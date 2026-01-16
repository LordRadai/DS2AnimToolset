#pragma once
#include "../Array/ArrayAttribute.inl"

namespace db
{
	class CompositeAttribute : public ArrayAttribute
	{
	protected:
		std::vector<Attribute*> m_attributes;

	public:
		CompositeAttribute(Attribute* parent, std::string name, std::string type) : 
			ArrayAttribute(parent, name, type) {};

		virtual ~CompositeAttribute() override {};
		virtual bool compare(Attribute* other) override;
		virtual void assign(Attribute* other) override;
		virtual CompositeAttribute* asComposite() const override { return const_cast<CompositeAttribute*>(this); }
		virtual bool isComposite() const override { return true; }
		virtual uint32_t size() const override { return m_attributes.size(); }
		virtual bool empty() const override { return m_attributes.empty(); }
		virtual void clearArray() override {};
		virtual void removeAt(int idx) override;
		virtual void resize(uint32_t newSize) override {};
		virtual bool writeStartArrayXML(int format, SaverXML* saver) const override { return false; }

		void deleteChildren();
		void insertAttribute(int idx, Attribute* attribute);
		void insertAttributeUnmanaged(int idx, Attribute* attribute);
		void removeAttribute(Attribute* attribute);
		void removeAttributeUnmanaged(Attribute* attribute);
		Attribute* back() const { return m_attributes.back(); }
		Attribute* front() const { return m_attributes.front(); }
		Attribute* getAttribute(int idx) const;
		Attribute* findAttribute(std::string name) const;
		uint32_t getAttributeCount() const { return static_cast<uint32_t>(m_attributes.size()); }

		int getAttributeIndex(Attribute* attribute) const;

		Attribute* operator[] (uint32_t idx) const
		{
			if (idx < 0 || idx >= static_cast<int>(m_attributes.size()))
				throw std::out_of_range("CompositeAttribute::operator[] - Index out of range");

			return m_attributes[idx];
		}
	};
}