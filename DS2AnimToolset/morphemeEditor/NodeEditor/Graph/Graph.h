#pragma once
#include <vector>
#include <string>
#include "NodeEditor/imnodes/imnodes.h"
#include "NodeEditor/Node/Node.h"
#include "NodeEditor/Link/Link.h"

namespace NodeEditor
{
	class Graph
	{
		int m_id;
		ImNodesContext* m_context;
		std::vector<Node*> m_nodes;
		std::vector<Link*> m_links;
	public:
		Graph();
		~Graph();

		int getID() const { return m_id; }

		Node* addNode(int nodeID, const std::string& name);
		Node* addContainerNode(int nodeID, const std::string& name);
		void removeNode(Node* node);

		void addLink(Link* link) { m_links.push_back(link); }

		const std::vector<Node*>& getNodes() const { return m_nodes; }
		const std::vector<Link*>& getLinks() const { return m_links; }

		virtual void draw();
	};
}
