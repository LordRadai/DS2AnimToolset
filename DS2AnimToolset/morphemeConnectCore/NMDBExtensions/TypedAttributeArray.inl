#pragma once
#include "NMDatabase/NMDatabase.h"

namespace db
{
	template<typename T>
	class TypedAttributeArray : public db::AttributeArray
	{
		static_assert(std::is_base_of<db::Attribute, T>::value, "T must be derived from db::Attribute");
	public:
		TypedAttributeArray(CompositeAttribute* parent, std::string name, std::string typeName)
			: AttributeArray(parent, name, typeName)
		{}

		T* add()
		{
			T* attr = insert(size());

			return attr;
		}

		T* insert(int idx)
		{
			T* attr = new T(this, "elem", 0);
			insertAttribute(idx, attr);
			return attr;
		}
	};
}
