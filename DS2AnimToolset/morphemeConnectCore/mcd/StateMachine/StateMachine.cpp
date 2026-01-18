#include "StateMachine.h"
#include "mcd/BlendTree/BlendTree.h"
#include "mcu/Log/Log.h"

namespace mcd
{
	StateMachine::StateMachine(db::CompositeAttribute* parent, const std::string& name, const std::string& nodeType, int manifestVersion)
		: Graph(parent, "StateMachine", name),
		m_nodeType(std::make_unique<db::StringAttribute>(this, "NodeType", nodeType)),
		m_manifestVersion(std::make_unique<db::IntAttribute>(this, "ManifestVersion", manifestVersion)),
		m_stateMachineNodes(std::make_unique<db::TypedNodeContainer<StateMachineNode>>(this, "StateMachineNodes")),
		m_transitionEdges(std::make_unique<db::TypedNodeContainer<TransitionEdge>>(this, "TransitionEdges")),
		m_defaultState(std::make_unique<db::Pointer<StateMachineNode>>(this, "DefaultState", nullptr))
	{
		addAttribute(m_nodeType.get());
		addAttribute(m_manifestVersion.get());
	}

	void StateMachine::addStateMachineNode(StateMachineNode* node)
	{
		if (node == nullptr)
			return;

		if (findAttribute("StateMachineNodes") == nullptr)
			addAttribute(m_stateMachineNodes.get());

		m_stateMachineNodes->add(node);

		if (getDefaultState() == nullptr)
			setDefaultState(node);
	}

	void StateMachine::addTransitionEdge(TransitionEdge* edge)
	{
		if (edge == nullptr)
			return;

		if (findAttribute("TransitionEdges") == nullptr)
			addAttribute(m_transitionEdges.get());

		m_transitionEdges->add(edge);
	}

	void StateMachine::setDefaultState(StateMachineNode* state)
	{
		removeAttribute(m_defaultState.get());

		m_defaultState->setValue(state);

		if (state)
			addAttribute(m_defaultState.get());
	}

	uint32_t StateMachine::getNumNodesOfType(const std::string& type) const
	{
		uint32_t count = 0;

		for (size_t i = 0; i < m_stateMachineNodes->size(); i++)
		{
			if (m_stateMachineNodes->getNode(i)->getNodeType() == type)
				count++;
		}

		return count;
	}

	uint32_t StateMachine::getNumTransitionOfType(const std::string& type) const
	{
		uint32_t count = 0;

		for (size_t i = 0; i < m_transitionEdges->size(); i++)
		{
			if (m_transitionEdges->getNode(i)->getEdgeType() == type)
				count++;
		}

		return count;
	}

	void StateMachine::getFreePosition(float& outX, float& outY)
	{
		outX = 10.0f;
		outY = 0.0f;

		float maxX = 0.0f;
		float maxY = 0.0f;

		for (size_t i = 0; i < m_stateMachineNodes->size(); i++)
		{
			StateMachineNode* node = m_stateMachineNodes->getNode(i);

			const float nodeX = node->getXPos();
			const float nodeY = node->getYPos();

			if (nodeX > maxX)
				maxX = nodeX;

			if (nodeY > maxY)
				maxY = nodeY;
		}

		outX = 10.0f;
		outY = maxY + 100.0f;
	}

	void StateMachine::updateCommonPin()
	{
		db::Node* parentNode = getParentNode();

		if (parentNode->isOfType<mcd::StateMachineNode>() && parentNode->hasParentNode<mcd::StateMachine>())
		{
			mcd::StateMachine* parentSM = static_cast<mcd::StateMachine*>(parentNode->getParentNode());
			parentSM->updateCommonPin();
		}
		else if (parentNode->isOfType<mcd::BlendTreeNode>())
		{
			// If the state machine is inside a blend tree, update the blend tree's common pin. To be implemented.
			LOG_TODO("Implement common pin update for StateMachine inside BlendTree");
		}

		return;
	}
}
