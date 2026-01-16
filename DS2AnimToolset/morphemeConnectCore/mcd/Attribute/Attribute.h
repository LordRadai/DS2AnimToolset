#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class Attribute : public db::Node
	{
	protected:
		Attribute(db::CompositeAttribute* parent, const std::string& name, const std::string& identifier) : db::Node(parent, name, identifier) {}
	public:
		virtual ~Attribute() {}
		virtual bool assignValue(Attribute* attr);
		virtual bool isValueEqualTo(Attribute* attr) { return false; };
	};
}
