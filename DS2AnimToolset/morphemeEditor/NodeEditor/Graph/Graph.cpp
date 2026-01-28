#include "Graph.h"
#include "NodeEditor/Registry/Registry.h"

namespace NodeEditor
{
	Graph::Graph(Graph* parent, const std::string& name) : m_parentGraph(parent), m_name(name), m_context(nullptr)
	{
		m_context = ImNodes::CreateContext();
		m_id = Registry::getInstance()->generateUniqueGraphID();
		Registry::getInstance()->registerGraph(this);
	}

	Graph::~Graph()
	{
		for (Node* node : m_nodes)
			delete node;

		Registry::getInstance()->unregisterGraph(this);
		ImNodes::DestroyContext(m_context);
	}

	Node* Graph::createNode(int nodeID, const std::string& name)
	{
		float x, y;
		getFreePosition(x, y);

		Node* node = new Node(this, nodeID, name, nullptr);
		node->setPosition(x, y);

		m_nodes.push_back(node);

		return node;
	}

	Node* Graph::createContainerNode(int nodeID, const std::string& name)
	{
		float x, y;
		getFreePosition(x, y);

		Node* node = new Node(this, nodeID, name, new Graph(this, name));
		node->setPosition(x, y);

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

		for (Link* link : m_links)
			link->draw();
	}

	const std::string Graph::getFullName() const
	{
		if (m_parentGraph)
			return m_parentGraph->getFullName() + "|" + m_name;
		else
			return m_name;
	}

	void Graph::getFreePosition(float& x, float& y)
	{
		float maxX = 0.f;
		float maxY = 0.f;

		for (const Node* node : m_nodes)
		{
			ImVec2 pos = node->getPosition();

			if (pos.x >= maxX)
				maxX = pos.x;

			if (pos.y >= maxY)
				maxY = pos.y;
		}

		x = 10.f;
		y = maxY + 100.f;
	}
}
