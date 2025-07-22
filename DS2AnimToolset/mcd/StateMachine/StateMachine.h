#pragma once
#include "Graph/Graph.h"
#include "GraphNode/GraphNode.h"

namespace mcd
{
	class StateMachine : public Graph
	{
		db::NodeContainer m_stateMachineNodes;
		db::NodeContainer m_transitions;
		GraphNode* m_defaultState;

	public:
		StateMachine()
	};
}
