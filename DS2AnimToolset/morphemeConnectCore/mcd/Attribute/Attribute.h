#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class Attribute : public db::Node
	{
	public:
		Attribute(db::Node* parent, const std::string& name)
			: db::Node(parent, name, "") {}

		virtual ~Attribute() {}
		virtual bool assignValue(Attribute* attr);
	};
}
