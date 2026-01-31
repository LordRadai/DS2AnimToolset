#include "StateMachine.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	StateMachine::StateMachine(NodeEditor* editor, Graph* parent, const std::string& name) : Graph(editor, parent, name)
	{
	}

	StateMachine::~StateMachine()
	{
		for (Transition* transition : m_transitions)
			delete transition;
	}

	void StateMachine::draw()
	{
		ImNodesStyle style = ImNodes::GetStyle();

		ImNodes::PushColorStyle(ImNodesCol_NodeBackground, style.Colors[ImNodesCol_TitleBar]);
		ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundHovered, style.Colors[ImNodesCol_TitleBarHovered]);
		ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, style.Colors[ImNodesCol_TitleBarSelected]);

		Graph::draw();

		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();

		for (Transition* transition : m_transitions)
			transition->draw();
	}

	StateNode* StateMachine::createStateNode(int nodeID, const std::string& name)
	{
		std::string nodeName = makeNameValid(name);

		StateNode* stateNode = new StateNode(m_ownerEditor, this, nodeID, nodeName, nullptr);
		m_nodes.push_back(stateNode);

		return stateNode;
	}

	Transition* StateMachine::getTransition(int nodeID) const
	{
		for (Transition* transition : m_transitions)
		{
			if (transition->getID() == nodeID)
				return transition;
		}

		return nullptr;
	}

	Transition* StateMachine::getTransition(const std::string& name) const
	{
		for (Transition* transition : m_transitions)
		{
			if (transition->getName() == name)
				return transition;
		}

		return nullptr;
	}

	Transition* StateMachine::createTransition(int nodeID, Node* sourceNode, Node* destinationNode)
	{
		Transition* transition = new Transition(m_ownerEditor, this, nodeID, sourceNode, destinationNode);
		m_transitions.push_back(transition);

		return transition;
	}
}
