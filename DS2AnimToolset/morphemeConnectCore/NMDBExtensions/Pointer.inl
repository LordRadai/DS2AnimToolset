#pragma once
#include "NMDatabase/NMDatabase.h"

namespace db
{
	template<typename T>
	class Pointer : public db::PointerAttribute
	{
		static_assert(std::is_base_of<db::Node, T>::value, "T must be derived from db::Node");
	public:
		Pointer(Attribute* parent, const std::string& name, T* value)
			: db::PointerAttribute(parent, name, value) {}

		virtual ~Pointer() override {};

		T* getValue() const { return dynamic_cast<T*>(db::PointerAttribute::getValue()); }
		void setValue(T* value) { db::PointerAttribute::setValue(value); }
	};
}
