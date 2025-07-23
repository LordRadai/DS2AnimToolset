#pragma once
#include "ArrayAttribute.inl"

namespace db
{
	class IntArrayAttribute : public ArrayAttribute
	{
		std::vector<bool> m_values;

	public:
		IntArrayAttribute(Attribute* parent, std::string name, std::string label) :
			ArrayAttribute(parent, name, label, "intArray") {
		};

		virtual ~IntArrayAttribute() override {};
		virtual bool writeValueXML(int format) override;
		virtual uint32_t size() const override { return m_values.size(); };
		virtual bool empty() const override { return m_values.empty(); };
		virtual void removeAt(int idx) override;
		virtual void clearArray() override { m_values.clear(); }
		virtual bool writeStartArrayXML(int format) const override;

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