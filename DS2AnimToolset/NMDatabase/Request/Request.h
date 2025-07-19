#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class Request : public Node
	{
	public:
		Request() : Node("Request") {}
		Request(std::string name) : Node(name) {}

		~Request() {};
	};
}