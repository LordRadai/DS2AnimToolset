#pragma once
#include "mcd/Graph/Graph.h"
#include "StateMachineNode.h"
#include "mcd/Transition/TransitionEdge.h"
#include "NMDBExtensions/Pointer.inl"
#include "NMDBExtensions/TypedNodeContainer.inl"

namespace mcd
{
	class StateMachine : public Graph
	{
		std::unique_ptr<db::StringAttribute> m_nodeType;
		std::unique_ptr<db::IntAttribute> m_manifestVersion;
		std::unique_ptr<db::TypedNodeContainer<StateMachineNode>> m_stateMachineNodes;
		std::unique_ptr<db::TypedNodeContainer<TransitionEdge>> m_transitionEdges;
		std::unique_ptr<db::Pointer<StateMachineNode>> m_defaultState;

	public:
		StateMachine(db::Attribute* parent, const std::string& name, const std::string& nodeType = "StateMachine", int manifestVersion = 1);

		virtual ~StateMachine() override {};

		const std::string& getNodeType() const { return m_nodeType->getValue(); }
		int getManifestVersion() const { return m_manifestVersion->getValue(); }
		void setNodeType(const std::string& type) { m_nodeType->setValue(type); }
		void setManifestVersion(int version) { m_manifestVersion->setValue(version); }

		void addStateMachineNode(StateMachineNode* node) { m_stateMachineNodes->add(node); }
		StateMachineNode* getStateMachineNode(const uint32_t index) const { return m_stateMachineNodes->getNode(index); }
		StateMachineNode* findStateMachineNode(const std::string& name) const { return m_stateMachineNodes->find(name); }

		void addTransitionEdge(TransitionEdge* edge) { m_transitionEdges->add(edge); }
		TransitionEdge* getTransitionEdge(const uint32_t index) const { return m_transitionEdges->getNode(index); }
		TransitionEdge* findTransitionEdge(const std::string& name) const { return m_transitionEdges->find(name); }

		StateMachineNode* getDefaultState() const { return m_defaultState->getValue(); }
		void setDefaultState(StateMachineNode* state) { m_defaultState->setValue(state); }
	};
}
