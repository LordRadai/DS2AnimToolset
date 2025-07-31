#pragma once
#include "NMDatabase/NMDatabase.h"

namespace db
{
	template<typename T>
	class TypedAttributeArray : public db::AttributeArray
	{
		static_assert(std::is_base_of<db::Attribute, T>::value, "T must be derived from db::Node");
	public:
		TypedAttributeArray(db::Attribute* parent, std::string name)
			: db::AttributeArray(parent, name)
		{}

		void add(T* value) { return AttributeArray::add(value); }
		void insert(T* value, int idx) { return AttributeArray::insert(value, idx); }
	};
}
