#include "Graph.h"
#include "BlendTree.h"
#include "StateMachine.h"

#include "NodeEditor/NodeEditor.h"

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
			if (node->getNodeID() == nodeID)
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
		float x, y;
		getFreePosition(x, y);
		
		return createNode(nodeID, typeName, name, x, y);
	}

	Node* Graph::createNode(int nodeID, const std::string& typeName, const std::string& name, float x, float y)
	{
		std::string nameToUse = name;

		if (nameToUse == "")
			nameToUse = typeName;

		std::string nodeName = makeNameValid(nameToUse, typeName);

		Manifest::MMNode* manifestNode = m_ownerEditor->getManifest()->findNodeManifest(typeName);

		if (!manifestNode)
			throw std::runtime_error("Graph::createNode: Node type '" + typeName + "' not found in Manifest.");

		Node* node = manifestNode->makeNode(m_ownerEditor, this, nodeID, nodeName);
		node->setPosition(x, y);

		m_nodes.push_back(node);

		return node;
	}

	Node* Graph::createBlendTree(int nodeID, const std::string& name, float x, float y)
	{
		std::string nameToUse = name;

		if (nameToUse == "")
			nameToUse = "BlendTree";

		std::string nodeName = makeNameValid(nameToUse, "BlendTree");

		Node* node = new Node(m_ownerEditor, this, nodeID, "BlendTree", nodeName, new BlendTree(m_ownerEditor, this, name));
		node->setPosition(x, y);

		m_nodes.push_back(node);

		node->createOutputPin("Result");

		return node;
	}

	Node* Graph::createBlendTree(int nodeID, const std::string& name)
	{
		float x, y;
		getFreePosition(x, y);

		return createBlendTree(nodeID, name, x, y);
	}

	Node* Graph::createStateMachine(int nodeID, const std::string& name, float x, float y)
	{
		std::string nameToUse = name;

		if (nameToUse == "")
			nameToUse = "StateMachine";

		std::string nodeName = makeNameValid(nameToUse, "StateMachine");

		Manifest::MMStateMachine* manifestNode = m_ownerEditor->getManifest()->findStateMachineManifest("StateMachine");

		if (!manifestNode)
			throw std::runtime_error("Graph::createStateMachine: StateMachine type 'StateMachine' not found in Manifest.");

		Node* node = manifestNode->makeNode(m_ownerEditor, this, nodeID, name);
		node->setPosition(x, y);

		m_nodes.push_back(node);

		node->setSubGraph(new StateMachine(m_ownerEditor, this, name));

		if (isOfType<BlendTree>())
			node->createOutputPin("Result");

		return node;
	}

	Node* Graph::createStateMachine(int nodeID, const std::string& name)
	{
		float x, y;
		getFreePosition(x, y);

		return createStateMachine(nodeID, name, x, y);
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

	void Graph::navigatorGui()
	{
		if (ImGui::TreeNodeEx(m_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (size_t i = 0; i < m_nodes.size(); i++)
			{
				Node* node = m_nodes[i];

				if (!node->hasSubGraph())
					ImGui::Selectable(node->getName().c_str());
				else
					node->getSubGraph()->navigatorGui();
			}

			ImGui::TreePop();
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
		if (m_parentGraph && !m_parentGraph->isRootGraph())
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

	const std::string Graph::makeNameValid(const std::string& desiredName, const std::string& typeName)
	{
		int numNodesWithName = 0;

		for (size_t i = 0; i < m_nodes.size(); i++)
		{
			Node* node = m_nodes[i];

			if (node->getTypeName() == typeName)
				numNodesWithName++;
		}

		std::string splitChar = "";

		if (std::isdigit(desiredName.back()))
			splitChar = "_";

		if (typeName == desiredName)
			return std::string(desiredName + splitChar +std::to_string(numNodesWithName + 1));

		return desiredName;
	}
}
