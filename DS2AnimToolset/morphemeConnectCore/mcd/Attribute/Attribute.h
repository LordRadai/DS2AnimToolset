#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class Attribute : public db::Node
	{
	public:
		Attribute(db::Node* parent, const std::string& name, const std::string& identifier)
			: db::Node(parent, name, identifier) {}

		virtual ~Attribute() {}
		virtual bool assignValue(Attribute* attr);
	};
}
