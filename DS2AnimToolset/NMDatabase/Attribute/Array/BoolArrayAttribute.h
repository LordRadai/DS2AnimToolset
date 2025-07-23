#pragma once
#include "ArrayAttribute.inl"

namespace db
{
	class BoolArrayAttribute : public ArrayAttribute
	{
		std::vector<bool> m_values;

	public:
		BoolArrayAttribute(Attribute* parent, std::string name, std::string label) :
			ArrayAttribute(parent, name, label, "boolArray") {
		};

		virtual ~BoolArrayAttribute() override {};
		virtual bool writeValueXML(int format) override;
		virtual uint32_t size() const override { return m_values.size(); };
		virtual bool empty() const override { return m_values.empty(); };
		virtual void removeAt(int idx) override;
		virtual void clearArray() override { m_values.clear(); }
		virtual bool writeStartArrayXML(int format) const override;

		void add(bool value) { m_values.push_back(value); }
		void insert(int idx, bool value);
		void setElement(int idx, bool value);
		bool getElement(int idx) const;

		bool operator[] (uint32_t idx) const
		{
			if (idx < 0 || idx >= static_cast<int>(m_values.size()))
				throw std::out_of_range("BoolArrayAttribute::operator[] - Index out of range");

			return m_values[idx];
		}
	};
}