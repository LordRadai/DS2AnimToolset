#pragma once
#include "ArrayAttribute.inl"

namespace db
{
	class IntArrayAttribute : public ArrayAttribute
	{
		std::vector<int> m_values;

	public:
		IntArrayAttribute(CompositeAttribute* parent, std::string name) :
			ArrayAttribute(parent, name, "intArray") {
		};

		virtual ~IntArrayAttribute() override {};
		virtual bool compare(Attribute* other) override;
		virtual void assign(Attribute* other) override;
		virtual IntArrayAttribute* asIntArray() const override { return const_cast<IntArrayAttribute*>(this); }
		virtual bool isIntArray() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;
		virtual uint32_t size() const override { return m_values.size(); };
		virtual bool empty() const override { return m_values.empty(); };
		virtual void removeAt(int idx) override;
		virtual void clearArray() override { m_values.clear(); }
		virtual void resize(uint32_t newSize) override { m_values.resize(newSize); }
		virtual bool writeStartArrayXML(int format, SaverXML* saver) const override;

		void add(int value) { m_values.push_back(value); }
		void insert(int idx, int value);
		void setElement(int idx, int value);
		int getElement(int idx) const;

		int operator[] (uint32_t idx) const
		{
			if (idx < 0 || idx >= static_cast<int>(m_values.size()))
				throw std::out_of_range("IntArrayAttribute::operator[] - Index out of range");

			return m_values[idx];
		}
	};
}