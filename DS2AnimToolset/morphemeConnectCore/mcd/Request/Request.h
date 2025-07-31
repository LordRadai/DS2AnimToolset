#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class Request : public db::Node
	{
	public:
		Request(db::Node* parent, const std::string& name) :
			db::Node(parent, name, "Request") {};

		virtual ~Request() override {};
	};
}
