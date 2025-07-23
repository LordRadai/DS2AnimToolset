#pragma once
#include "RCore.h"
#include "Attribute/AttributeContainer.h"
#include "Node/Node.h"

namespace db
{
	class NodeContainer : public AttributeContainer
	{
	public:
		NodeContainer(Node* parent, std::string identifier) : 
			AttributeContainer(parent, identifier, "nodeContainer") {};

		virtual ~NodeContainer() override {};
		virtual bool writeValueXML(int format) override;

		void add(Node* node) { insertAttribute(getNumNodes() - 1, node); }
		void remove(Attribute* attribute) { removeAttribute(attribute); }
		Node* getNode(int idx) const { return dynamic_cast<Node*>(getAttribute(idx)); }
		Node* find(std::string name) const { Attribute* attr = findAttribute(name); }
		bool hasNode(Node* node) const { return find(node->getName()) != nullptr; }
		Node* front() const { return dynamic_cast<Node*>(AttributeContainer::front()); }
		Node* back() const { return dynamic_cast<Node*>(AttributeContainer::back()); }
		size_t getNumNodes() const { return AttributeContainer::size(); }
	};
}