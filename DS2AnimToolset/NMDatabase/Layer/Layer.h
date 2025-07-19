#pragma once
#include "../Node/Node.h"

namespace db
{
	class Layer : public Node
	{
	public:
		Layer() : Node("Layer") {};
		Layer(const std::string& name) : Node(name) {};

		~Layer() {};
	};
}