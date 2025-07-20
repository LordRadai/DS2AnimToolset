#pragma once
#include "../Node/Node.h"

namespace db
{
	class Layer : public Node
	{
	public:
		Layer(Node* parent, const std::string name) : Node(parent, "Layer", name) {};

		virtual ~Layer() {};
		virtual bool isValid() const { return Node::isValid(); }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);
	};
}