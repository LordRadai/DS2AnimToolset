#pragma once
#include "Node/Node.h"

namespace db
{
	class Database : public Node
	{
	public:
		Database(std::string identifier, std::string name) : Node(nullptr, identifier, name) {};

		virtual ~Database() override {};

		void add(Node* node) { insertAttribute(getNumNodes(), node); }
		void remove(Attribute* attribute) { removeAttribute(attribute); }
		Node* getNode(int idx) const { return dynamic_cast<Node*>(getAttribute(idx)); }
		Node* find(std::string name) const { return dynamic_cast<Node*>(findAttribute(name)); }
		bool hasNode(Node* node) const { return find(node->getName()) != nullptr; }
		Node* front() const { return dynamic_cast<Node*>(Node::front()); }
		Node* back() const { return dynamic_cast<Node*>(Node::back()); }
		size_t getNumNodes() const { return Node::size(); }
	};
}