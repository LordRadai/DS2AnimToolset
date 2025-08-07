#pragma once
#include "RCore.h"
#include "Attribute/AttributeContainer/AttributeContainer.h"
#include "Node/Node.h"

namespace db
{
	class NodeContainer : public AttributeContainer
	{
	public:
		NodeContainer(Node* parent, std::string identifier) : 
			AttributeContainer(parent, identifier, "nodeContainer") {};

		virtual ~NodeContainer() override {};
		virtual bool readValueXML(int format, db::XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;
		virtual void resize(uint32_t newSize) override {}

		void add(Node* node) { insertAttribute(getNumNodes(), node); }
		void remove(Attribute* attribute) { removeAttribute(attribute); }
		Node* getNode(int idx) const { return getAttribute(idx)->asNode(); }
		Node* find(std::string name) const { return findAttribute(name)->asNode(); }
		bool hasNode(Node* node) const { return find(node->getName()) != nullptr; }
		Node* front() const { return AttributeContainer::front()->asNode(); }
		Node* back() const { return AttributeContainer::back()->asNode(); }
		size_t getNumNodes() const { return AttributeContainer::size(); }
	};
}