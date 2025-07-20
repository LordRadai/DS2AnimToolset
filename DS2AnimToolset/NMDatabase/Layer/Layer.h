#pragma once
#include "../Node/Node.h"

namespace db
{
	class Layer : public Node
	{
	public:
		Layer(Node* parent, const std::string name) : Node(parent, name) {};
		~Layer() {};
	};
}