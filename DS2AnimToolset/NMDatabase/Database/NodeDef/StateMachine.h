#pragma once
#include "NodeDef.h"
#include "../NodeContainer/NodeContainer.h"
#include "../TransitionEdge/TransitionEdge.h"

namespace db
{
	class StateMachine : public NodeDef
	{
		NodeContainer m_stateMachineNodes;
		float m_panX;
		float m_panY;
		NodeContainer m_transitions;
		NodeDef* m_defaultState;

	public:
		StateMachine(Node* parent, std::string name, std::string identifier);

		virtual ~StateMachine() {}
		virtual bool isValid() const override;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) override;
	};
}
