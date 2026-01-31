#pragma once
#include "Graph.h"
#include "NodeEditor/StateNode/StateNode.h"

namespace NodeEditor
{
	class StateMachine : public Graph
	{
		std::vector<Transition*> m_transitions;

	public:
		StateMachine(NodeEditor* editor, Graph* parent, const std::string& name);
		~StateMachine() override;

		void draw() override;

		StateNode* createStateNode(int nodeID, const std::string& name = "ActiveState");

		Transition* getTransition(int nodeID) const;
		Transition* getTransition(const std::string& name) const;

		Transition* createTransition(int nodeID, Node* sourceNode, Node* destinationNode);
	};
}
