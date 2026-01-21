#pragma once
#include "Pointer.inl"

namespace db
{
	template<typename T>
	class PointerArray : public db::PointerArrayAttribute
	{
	public:
		PointerArray(db::Attribute* parent, const std::string& name)
			: db::PointerArrayAttribute(parent, name)
		{}
	};
}
