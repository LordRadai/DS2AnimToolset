#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class NodeContainer : public Node
	{
	protected:
		std::vector<Node*> m_nodes;

	public:
		NodeContainer(Node* parent, std::string identifier) : Node(parent, identifier, "") {};

		virtual ~NodeContainer() {};
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);
		virtual std::string getQualifiedName() const;

		void addNode(Node* node);
		void removeNode(int index);
		Node* getNode(int index) const;
		Node* findNodeByName(const std::string& name) const;
		size_t getNumNodes() const { return m_nodes.size(); }
	};
}