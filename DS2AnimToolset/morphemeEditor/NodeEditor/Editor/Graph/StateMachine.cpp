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
		std::string nodeName = makeNameValid(name, "ActiveState");

		Manifest::MMStateMachineNode* manifestNode = m_ownerEditor->getManifest()->findStateMachineNodeManifest("ActiveState");

		StateNode* stateNode = manifestNode->makeNode(m_ownerEditor, this, nodeID, nodeName);
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

	Transition* StateMachine::createTransition(int nodeID, const std::string& typeName, Node* sourceNode, Node* destinationNode)
	{
		Manifest::MMTransition* manifestTransition = m_ownerEditor->getManifest()->findTransitionManifest(typeName);
		
		if (manifestTransition == nullptr)
			throw std::runtime_error("StateMachine::createTransition: Transition type '" + typeName + "' not found in manifest.");

		Transition* transit = manifestTransition->makeTransition(m_ownerEditor, nodeID, sourceNode, destinationNode);

		m_transitions.push_back(transit);

		return transit;
	}
}
