#pragma once
#include "ArrayAttribute.inl"
#include "NMPlatform/NMQuat.h"

namespace db
{
	class QuaternionArrayAttribute : public ArrayAttribute
	{
		std::vector<NMP::Quat> m_values;

	public:
		QuaternionArrayAttribute(Attribute* parent, std::string name) :
			ArrayAttribute(parent, name, "quaternionArray") {
		};

		virtual ~QuaternionArrayAttribute() override {};
		virtual bool compare(Attribute* other) override;
		virtual void assign(Attribute* other) override;
		virtual bool writeValueXML(int format) override;
		virtual uint32_t size() const override { return m_values.size(); };
		virtual bool empty() const override { return m_values.empty(); };
		virtual void removeAt(int idx) override;
		virtual void clearArray() override { m_values.clear(); }
		virtual void resize(uint32_t newSize) override { m_values.resize(newSize); }
		virtual bool writeStartArrayXML(int format) const override;

		void add(NMP::Quat value) { m_values.push_back(value); }
		void insert(int idx, NMP::Quat value);
		void setElement(int idx, NMP::Quat value);
		NMP::Quat getElement(int idx) const;

		NMP::Quat operator[] (uint32_t idx) const
		{
			if (idx < 0 || idx >= static_cast<int>(m_values.size()))
				throw std::out_of_range("QuaternionArrayAttribute::operator[] - Index out of range");

			return m_values[idx];
		}
	};
}