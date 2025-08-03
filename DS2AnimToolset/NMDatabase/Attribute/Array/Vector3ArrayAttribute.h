#pragma once
#include "ArrayAttribute.inl"
#include "NMPlatform/NMVector3.h"

namespace db
{
	class Vector3ArrayAttribute : public ArrayAttribute
	{
		std::vector<NMP::Vector3> m_values;

	public:
		Vector3ArrayAttribute(Attribute* parent, std::string name) :
			ArrayAttribute(parent, name, "boolArray") {
		};

		virtual ~Vector3ArrayAttribute() override {};
		virtual bool compare(Attribute* other) override;
		virtual void assign(Attribute* other) override;
		virtual Vector3ArrayAttribute* asVector3Array() const override { return const_cast<Vector3ArrayAttribute*>(this); }
		virtual bool isVector3Array() const override { return true; }
		virtual bool readValueXML(int format, db::XMLElement* element) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;
		virtual uint32_t size() const override { return m_values.size(); };
		virtual bool empty() const override { return m_values.empty(); };
		virtual void removeAt(int idx) override;
		virtual void clearArray() override { m_values.clear(); }
		virtual void resize(uint32_t newSize) override { m_values.resize(newSize); }
		virtual bool writeStartArrayXML(int format, SaverXML* saver) const override;

		void add(NMP::Vector3 value) { m_values.push_back(value); }
		void insert(int idx, NMP::Vector3 value);
		void setElement(int idx, NMP::Vector3 value);
		NMP::Vector3 getElement(int idx) const;

		NMP::Vector3 operator[] (uint32_t idx) const
		{
			if (idx < 0 || idx >= static_cast<int>(m_values.size()))
				throw std::out_of_range("Vector3ArrayAttribute::operator[] - Index out of range");

			return m_values[idx];
		}
	};
}