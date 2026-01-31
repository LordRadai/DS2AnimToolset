#include "Graph.h"
#include "NodeEditor/Registry/Registry.h"
#include "NodeEditor/NodeEditor.h"
#include "BlendTree.h"
#include "StateMachine.h"
#include "extern.h"
#include "morpheme/mrDefines.h"s
#include "RLog/RLog.h"

namespace NodeEditor
{
	Graph::Graph(NodeEditor* editor, Graph* parent, const std::string& name) : Entity(editor, name), m_parentGraph(parent), m_context(nullptr)
	{
		Registry* registry = m_ownerEditor->getRegistry();

		m_context = ImNodes::CreateContext();
		m_id = registry->generateUniqueGraphID();
		registry->registerGraph(this);
	}

	Graph::~Graph()
	{
		for (Node* node : m_nodes)
			delete node;

		Registry* registry = m_ownerEditor->getRegistry();

		registry->unregisterGraph(this);
		ImNodes::DestroyContext(m_context);
	}

	Node* Graph::getNode(int nodeID) const
	{
		for (Node* node : m_nodes)
		{
			if (node->getID() == nodeID)
				return node;
		}

		return nullptr;
	}

	Node* Graph::getNode(const std::string& name) const
	{
		for (Node* node : m_nodes)
		{
			if (node->getName() == name)
				return node;
		}

		return nullptr;
	}

	void Graph::setPanning(float x, float y)
	{
		m_panning = ImVec2(x, y);
	}

	Node* Graph::createNode(int nodeID, const std::string& typeName, const std::string& name)
	{
		if (!isOfType<BlendTree>())
		{
			g_appLog->panicMessage("Graph::createNode: Attempted to create a standard node within a non BlendTree graph (%s).\n", m_name);
			return nullptr;
		}

		float x, y;
		getFreePosition(x, y);

		std::string nameToUse = name;

		if (nameToUse == "")
			nameToUse = typeName;

		std::string nodeName = makeNameValid(nameToUse);

		Node* node = new Node(m_ownerEditor, this, nodeID, typeName, nodeName, nullptr);
		node->setPosition(x, y);

		m_nodes.push_back(node);

		return node;
	}

	Node* Graph::createBlendTree(int nodeID, const std::string& name)
	{
		float x, y;
		getFreePosition(x, y);

		Node* node = new Node(m_ownerEditor, this, nodeID, "BlendTree", name, new BlendTree(m_ownerEditor, this, name));
		node->setPosition(x, y);

		m_nodes.push_back(node);

		return node;
	}

	Node* Graph::createStateMachine(int nodeID, const std::string& name)
	{
		float x, y;
		getFreePosition(x, y);

		Node* node = new Node(m_ownerEditor, this, nodeID, "StateMachine", name, new StateMachine(m_ownerEditor, this, name));
		node->setPosition(x, y);

		if (isOfType<BlendTree>())
			node->createOutputPin("Result");

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

		m_panning = ImNodes::EditorContextGetPanning();
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

	const std::string Graph::makeNameValid(const std::string& desiredName)
	{
		int numNodesWithName = 0;

		for (size_t i = 0; i < m_nodes.size(); i++)
		{
			Node* node = m_nodes[i];

			if (node->getName() == desiredName)
				numNodesWithName++;
		}

		return std::string(desiredName + std::to_string(numNodesWithName));
	}
}
