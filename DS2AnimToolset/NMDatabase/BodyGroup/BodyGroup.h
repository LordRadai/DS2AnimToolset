#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class BodyGroup : public Node
	{
	public:
		BodyGroup(Node* parent, const std::string& name) : Node(parent, name) {};

		virtual ~BodyGroup() {};
		virtual bool isValid() const { return Node::isValid(); }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		std::string getName() const { return m_name; }
	};
}