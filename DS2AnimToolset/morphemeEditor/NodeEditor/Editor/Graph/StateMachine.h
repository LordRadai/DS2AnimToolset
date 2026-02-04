#pragma once
#include "Graph.h"
#include "NodeEditor/Editor/Node/StateNode.h"
#include "NodeEditor/Editor/Transition/Transition.h"

namespace NodeEditor
{
	class StateMachine : public Graph
	{
		std::vector<StateNode*> m_stateNodes;
		std::vector<Transition*> m_transitions;
		int m_defaultNodeID;

	public:
		StateMachine(Editor* editor, Graph* parent, const std::string& name, int graphNodeID);
		~StateMachine() override;

		virtual void draw() override;
		virtual void updateNodePositions() override;

		int getDefaultNodeID() const { return m_defaultNodeID; }
		void setDefaultNodeID(int nodeID) { m_defaultNodeID = nodeID; }

		StateNode* getStateNode(const std::string& name) const;
		StateNode* getStateNodeAt(size_t index) const;
		size_t getNumStateNodes() const { return m_stateNodes.size(); }
		StateNode* getDefaultStateNode() const;
		StateNode* createStateNode(const std::string& name = "ActiveState");

		Node* getDefaultNode() const { return getNode(m_defaultNodeID); }

		Transition* getTransition(int nodeID) const;
		Transition* getTransition(const std::string& name) const;

		Transition* createTransition(int nodeID, const std::string& typeName, Node* sourceNode, Node* destinationNode);

	private:
		void drawDefaultStateMarker();
	};
}
