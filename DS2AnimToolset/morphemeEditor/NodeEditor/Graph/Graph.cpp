#include "Graph.h"
#include "NodeEditor/Registry/Registry.h"

namespace NodeEditor
{
	Graph::Graph()
	{
		m_context = ImNodes::CreateContext();
		Registry::getInstance()->registerGraph(this);
	}

	Graph::~Graph()
	{
		for (Node* node : m_nodes)
			delete node;
	}

	Node* Graph::addNode(int nodeID, const std::string& name)
	{
		Node* node = new Node(this, nodeID, name, nullptr);
		m_nodes.push_back(node);

		return node;
	}

	Node* Graph::addContainerNode(int nodeID, const std::string& name)
	{
		Node* node = new Node(this, nodeID, name, new Graph());
		m_nodes.push_back(node);

		return node;
	}

	void Graph::removeNode(Node* node)
	{
		auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
		if (it != m_nodes.end())
		{
			delete *it;
			m_nodes.erase(it);
		}
	}

	void Graph::draw()
	{
		for (Node* node : m_nodes)
			node->draw();
	}
}
