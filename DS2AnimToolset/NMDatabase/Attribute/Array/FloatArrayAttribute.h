#pragma once
#include "ArrayAttribute.inl"

namespace db
{
	class FloatArrayAttribute : public ArrayAttribute
	{
		std::vector<float> m_values;

	public:
		FloatArrayAttribute(Attribute* parent, std::string name, std::string label) :
			ArrayAttribute(parent, name, label, "floatArray") {
		};

		virtual ~FloatArrayAttribute() override {};
		virtual bool writeValueXML(int format) override;
		virtual uint32_t size() const override { return m_values.size(); };
		virtual bool empty() const override { return m_values.empty(); };
		virtual void removeAt(int idx) override;
		virtual void clearArray() override { m_values.clear(); }
		virtual bool writeStartArrayXML(int format) const override;

		void add(float value) { m_values.push_back(value); }
		void insert(int idx, float value);
		void setElement(int idx, float value);
		float getElement(int idx) const;

		float operator[] (uint32_t idx) const
		{
			if (idx < 0 || idx >= static_cast<int>(m_values.size()))
				throw std::out_of_range("FloatArrayAttribute::operator[] - Index out of range");

			return m_values[idx];
		}
	};
}