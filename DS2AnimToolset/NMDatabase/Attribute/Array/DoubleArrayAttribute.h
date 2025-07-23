#pragma once
#include "ArrayAttribute.inl"

namespace db
{
	class DoubleArrayAttribute : public ArrayAttribute
	{
		std::vector<double> m_values;

	public:
		DoubleArrayAttribute(Attribute* parent, std::string name, std::string label) :
			ArrayAttribute(parent, name, label, "doubleArray") {
		};

		virtual ~DoubleArrayAttribute() override {};
		virtual bool writeValueXML(int format) override;
		virtual uint32_t size() const override { return m_values.size(); };
		virtual bool empty() const override { return m_values.empty(); };
		virtual void removeAt(int idx) override;
		virtual void clearArray() override { m_values.clear(); }
		virtual bool writeStartArrayXML(int format) const override;

		void add(double value) { m_values.push_back(value); }
		void insert(int idx, double value);
		void setElement(int idx, double value);
		double getElement(int idx) const;

		double operator[] (uint32_t idx) const
		{
			if (idx < 0 || idx >= static_cast<int>(m_values.size()))
				throw std::out_of_range("DoubleArrayAttribute::operator[] - Index out of range");

			return m_values[idx];
		}
	};
}