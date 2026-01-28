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
		Graph* m_parentGraph;
		int m_id;
		std::string m_name;
		ImNodesContext* m_context;
		std::vector<Node*> m_nodes;
		std::vector<Link*> m_links;
	public:
		Graph(Graph* parent, const std::string& name);
		~Graph();

		void draw();

		Graph* getParentGraph() const { return m_parentGraph; }
		bool isRootGraph() const { return m_parentGraph == nullptr; }

		Node* createNode(int nodeID, const std::string& name);
		Node* createContainerNode(int nodeID, const std::string& name);
		void removeNode(Node* node);

		void addLink(Link* link) { m_links.push_back(link); }

		const int getID() const { return m_id; }
		const std::string& getName() const { return m_name; }
		const std::vector<Node*>& getNodes() const { return m_nodes; }
		const std::vector<Link*>& getLinks() const { return m_links; }

		const std::string getFullName() const;

	private:
		void getFreePosition(float& x, float& y);
	};
}
