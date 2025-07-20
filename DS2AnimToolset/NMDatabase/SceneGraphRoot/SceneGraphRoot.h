#pragma once
#include "../Node/Node.h"

namespace db
{
	class SceneGraphRoot : public Node
	{
	public:
		SceneGraphRoot(Node* parent) : Node(parent, "SceneGraphRoot") {};
		virtual ~SceneGraphRoot() {};
		virtual bool isValid() const { return Node::isValid(); }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) { return Node::serialize(parent); }
	};
}