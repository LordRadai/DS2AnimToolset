#include "StateMachine.h"

namespace NodeEditor
{
	StateMachine::StateMachine(NodeEditorBase* editor, Graph* parent, const std::string& name) : Graph(editor, parent, name)
	{
	}

	StateMachine::~StateMachine()
	{
		for (Transition* transition : m_transitions)
			delete transition;
	}

	void StateMachine::draw()
	{
		Graph::draw();

		for (Transition* transition : m_transitions)
			transition->draw();
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
