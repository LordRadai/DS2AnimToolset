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
		StateMachine(db::CompositeAttribute* parent, const std::string& name, const std::string& nodeType, int manifestVersion);

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

		/**
		 * \brief Returns the number of nodes with the specified type.
		 *
		 * \param type The node type name.
		 * \return The number of nodes of the specified type.
		 */
		uint32_t getNumNodesOfType(const std::string& type) const;

		/**
		 * \brief Returns the number of transitions with the specified type.
		 *
		 * \param type The transition type name.
		 * \return The number of transitions of the specified type.
		 */
		uint32_t getNumTransitionOfType(const std::string& type) const;

		/**
		 * \brief Returns the first free position in the graph.
		 *
		 * \param x Output buffer for the x position.
		 * \param y Output buffer for the y position.
		 */
		void getFreePosition(float& x, float& y);
	};
}
