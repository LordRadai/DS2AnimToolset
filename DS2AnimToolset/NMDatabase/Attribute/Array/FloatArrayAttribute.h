#pragma once
#include "ArrayAttribute.inl"

namespace db
{
	class FloatArrayAttribute : public ArrayAttribute
	{
		std::vector<float> m_values;

	public:
		FloatArrayAttribute(CompositeAttribute* parent, std::string name) :
			ArrayAttribute(parent, name, "floatArray") {
		};

		virtual ~FloatArrayAttribute() override {};
		virtual bool compare(Attribute* other) override;
		virtual void assign(Attribute* other) override;
		virtual FloatArrayAttribute* asFloatArray() const override { return const_cast<FloatArrayAttribute*>(this); }
		virtual bool isFloatArray() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;
		virtual uint32_t size() const override { return m_values.size(); };
		virtual bool empty() const override { return m_values.empty(); };
		virtual void removeAt(int idx) override;
		virtual void clearArray() override { m_values.clear(); }
		virtual void resize(uint32_t newSize) override { m_values.resize(newSize); }
		virtual bool writeStartArrayXML(int format, SaverXML* saver) const override;

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