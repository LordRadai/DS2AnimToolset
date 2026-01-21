#pragma once
#include "NMDatabase/NMDatabase.h"

namespace db
{
	template<typename T>
	class TypedAttributeArray : public db::AttributeArray
	{
		static_assert(std::is_base_of<db::Attribute, T>::value, "T must be derived from db::Node");
	public:
		TypedAttributeArray(CompositeAttribute* parent, std::string name)
			: AttributeArray(parent, name)
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
