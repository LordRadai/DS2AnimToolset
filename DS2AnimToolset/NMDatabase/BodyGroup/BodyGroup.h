#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class BodyGroup : public Node
	{
	public:
		BodyGroup(Node* parent, const std::string& name) : Node(parent, name) {};
		~BodyGroup() {};

		std::string getName() const { return m_name; }
	};
}