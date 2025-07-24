#pragma once
#include "ArrayAttribute.inl"
#include "NMPlatform/NMMatrix34.h"

namespace db
{
	class Matrix34ArrayAttribute : public ArrayAttribute
	{
		std::vector<NMP::Matrix34> m_values;

	public:
		Matrix34ArrayAttribute(Attribute* parent, std::string name) :
			ArrayAttribute(parent, name, "matrix34Array") {
		};

		virtual ~Matrix34ArrayAttribute() override {};
		virtual bool compare(Attribute* other) override;
		virtual void assign(Attribute* other) override;
		virtual Matrix34ArrayAttribute* asMatrix34Array() const override { return const_cast<Matrix34ArrayAttribute*>(this); }
		virtual bool isMatrix34Array() const override { return true; }
		virtual bool writeValueXML(int format) override;
		virtual uint32_t size() const override { return m_values.size(); };
		virtual bool empty() const override { return m_values.empty(); };
		virtual void removeAt(int idx) override;
		virtual void clearArray() override { m_values.clear(); }
		virtual void resize(uint32_t newSize) override { m_values.resize(newSize); }
		virtual bool writeStartArrayXML(int format) const override;

		void add(NMP::Matrix34 value) { m_values.push_back(value); }
		void insert(int idx, NMP::Matrix34 value);
		void setElement(int idx, NMP::Matrix34 value);
		NMP::Matrix34 getElement(int idx) const;

		NMP::Matrix34 operator[] (uint32_t idx) const
		{
			if (idx < 0 || idx >= static_cast<int>(m_values.size()))
				throw std::out_of_range("Matrix34ArrayAttribute::operator[] - Index out of range");

			return m_values[idx];
		}
	};
}