#pragma once
#include <vector>
#include <string>
#include "NodeEditor/imnodes/imnodes.h"
#include "NodeEditor/Editor/Node/Node.h"
#include "NodeEditor/Editor/Link/Link.h"

namespace NodeEditor
{
	class Graph : public Entity
	{
	protected:
		Graph* m_parentGraph;
		ImNodesContext* m_context;
		std::vector<Node*> m_nodes;
		ImVec2 m_panning;
		int m_graphNodeID;
	public:
		Graph(Editor* editor, Graph* parent, const std::string& name, int graphNodeID);
		~Graph();

		virtual void draw() override;
		virtual const std::string getFullName() const override;

		int getGraphNodeID() const { return m_graphNodeID; }

		Graph* getParentGraph() const { return m_parentGraph; }
		bool isRootGraph() const { return m_parentGraph == nullptr; }

		ImVec2 getPanning() const { return m_panning; }
		void setPanning(float x, float y);

		Node* getNodeAt(size_t index) const;
		Node* getNode(int nodeID) const;
		Node* getNode(const std::string& name) const;
		size_t getNumNodes() const { return m_nodes.size(); }

		Node* createStateMachine(int nodeID, const std::string& name = "");
		Node* createStateMachine(int nodeID, const std::string& name, float x, float y);
		Node* createBlendTree(int nodeID, const std::string& name = "");
		Node* createBlendTree(int nodeID, const std::string& name, float x, float y);

		void removeNode(Node* node);

		const std::vector<Node*>& getNodes() const { return m_nodes; }

		void navigatorGui();
	protected:
		Node* createNode(int nodeID, const std::string& typeName, const std::string& name);
		Node* createNode(int nodeID, const std::string& typeName, const std::string& name, float x, float y);

		void getFreePosition(float& x, float& y);

		const std::string makeNameValid(const std::string& desiredName, const std::string& typeName);
	};
}
