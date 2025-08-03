#pragma once
#include "ArrayAttribute.inl"

namespace db
{
	class EnumArrayAttribute : public ArrayAttribute
	{
		std::vector<std::string> m_values;

	public:
		EnumArrayAttribute(Attribute* parent, std::string name) :
			ArrayAttribute(parent, name, "enumArray") {
		};

		virtual ~EnumArrayAttribute() override {};
		virtual bool compare(Attribute* other) override;
		virtual void assign(Attribute* other) override;
		virtual EnumArrayAttribute* asEnumArray() const override { return const_cast<EnumArrayAttribute*>(this); }
		virtual bool isEnumArray() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;
		virtual uint32_t size() const override { return m_values.size(); };
		virtual bool empty() const override { return m_values.empty(); };
		virtual void removeAt(int idx) override;
		virtual void clearArray() override { m_values.clear(); }
		virtual void resize(uint32_t newSize) override { m_values.resize(newSize); }
		virtual bool writeStartArrayXML(int format, SaverXML* saver) const override;

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