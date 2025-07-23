#pragma once
#include "ArrayAttribute.inl"

namespace db
{
	class EnumArrayAttribute : public ArrayAttribute
	{
		std::vector<std::string> m_values;

	public:
		EnumArrayAttribute(Attribute* parent, std::string name, std::string label) :
			ArrayAttribute(parent, name, label, "enumArray") {
		};

		virtual ~EnumArrayAttribute() override {};
		virtual bool writeValueXML(int format) override;
		virtual uint32_t size() const override { return m_values.size(); };
		virtual bool empty() const override { return m_values.empty(); };
		virtual void removeAt(int idx) override;
		virtual void clearArray() override { m_values.clear(); }
		virtual bool writeStartArrayXML(int format) const override;

		void add(std::string value) { m_values.push_back(value); }
		void insert(int idx, std::string value);
		void setElement(int idx, std::string value);
		std::string getElement(int idx) const;

		std::string operator[] (uint32_t idx) const
		{
			if (idx < 0 || idx >= static_cast<int>(m_values.size()))
				throw std::out_of_range("EnumArrayAttribute::operator[] - Index out of range");

			return m_values[idx];
		}
	};
}