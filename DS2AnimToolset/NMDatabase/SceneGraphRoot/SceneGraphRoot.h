#pragma once
#include "../Node/Node.h"

namespace db
{
	class SceneGraphRoot : public Node
	{
	public:
		SceneGraphRoot(Node* parent) : Node(parent, "SceneGraphRoot") {};
		~SceneGraphRoot() {};
	};
}