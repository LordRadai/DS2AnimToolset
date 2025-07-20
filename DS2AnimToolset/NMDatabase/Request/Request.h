#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class Request : public Node
	{
	public:
		Request(Node* parent, std::string name) : Node(parent, name) {}
		~Request() {};
	};
}