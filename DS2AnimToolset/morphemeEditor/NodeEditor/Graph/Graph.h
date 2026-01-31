#pragma once
#include <vector>
#include <string>
#include "NodeEditor/imnodes/imnodes.h"
#include "NodeEditor/Node/Node.h"
#include "NodeEditor/Transition/Transition.h"
#include "NodeEditor/Link/Link.h"

namespace NodeEditor
{
	class Graph : public Entity
	{
	protected:
		Graph* m_parentGraph;
		ImNodesContext* m_context;
		std::vector<Node*> m_nodes;
		ImVec2 m_panning;
	public:
		Graph(NodeEditor* editor, Graph* parent, const std::string& name);
		~Graph();

		void draw();

		Graph* getParentGraph() const { return m_parentGraph; }
		bool isRootGraph() const { return m_parentGraph == nullptr; }

		ImVec2 getPanning() const { return m_panning; }
		void setPanning(float x, float y);

		Node* getNode(int nodeID) const;
		Node* getNode(const std::string& name) const;

		Node* createStateMachine(int nodeID, const std::string& name = "");
		Node* createStateMachine(int nodeID, const std::string& name, float x, float y);
		Node* createBlendTree(int nodeID, const std::string& name = "");
		Node* createBlendTree(int nodeID, const std::string& name, float x, float y);

		void removeNode(Node* node);

		const int getID() const { return m_id; }
		const std::string& getName() const { return m_name; }
		const std::vector<Node*>& getNodes() const { return m_nodes; }

		const std::string getFullName() const;

	protected:
		Node* createNode(int nodeID, const std::string& typeName, const std::string& name);
		Node* createNode(int nodeID, const std::string& typeName, const std::string& name, float x, float y);

		void getFreePosition(float& x, float& y);

		const std::string makeNameValid(const std::string& desiredName, const std::string& typeName);
	};
}
