#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class Request : public Node
	{
	public:
		Request(Node* parent, std::string name) : Node(parent, name) {}

		virtual ~Request() {};
		virtual bool isValid() const { Node::isValid(); }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);
	};
}