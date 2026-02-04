#pragma once
#include "Graph.h"
#include "NodeEditor/Editor/Node/StateNode.h"
#include "NodeEditor/Editor/Transition/Transition.h"

namespace NodeEditor
{
	class StateMachine : public Graph
	{
		std::vector<Transition*> m_transitions;
		int m_defaultNodeID;

	public:
		StateMachine(Editor* editor, Graph* parent, const std::string& name, int graphNodeID);
		~StateMachine() override;

		virtual void draw() override;

		int getDefaultNodeID() const { return m_defaultNodeID; }
		void setDefaultNodeID(int nodeID) { m_defaultNodeID = nodeID; }

		StateNode* createStateNode(int nodeID, const std::string& name = "ActiveState");

		Transition* getTransition(int nodeID) const;
		Transition* getTransition(const std::string& name) const;

		Transition* createTransition(int nodeID, const std::string& typeName, Node* sourceNode, Node* destinationNode);

	private:
		void drawDefaultStateMarker();
	};
}
