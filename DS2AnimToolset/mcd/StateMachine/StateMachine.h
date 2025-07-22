#pragma once
#include "Graph/Graph.h"
#include "GraphNode/GraphNode.h"

namespace mcd
{
	class StateMachine : public Graph
	{
		db::NodeContainer m_stateMachineNodes;
		db::NodeContainer m_transitionEdges;
		GraphNode* m_defaultState;

	public:
		StateMachine(db::Node* parent, std::string identifier, std::string label, std::string type, uint32_t version) :
			Graph(parent, identifier, label, type, version),
			m_stateMachineNodes(this, "StateMachineNodes"),
			m_transitionEdges(this, "TransitionEdges"),
			m_defaultState(nullptr)
		{};

		virtual ~StateMachine() override {};
		virtual bool isValid() const override { return m_defaultState != nullptr; }	
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) override;

		//StateMachineNode* getStateMachineNode(const std::string& name) const;
		size_t getNumStateMachineNodes() const { return m_stateMachineNodes.getNumNodes(); }
	};
}
