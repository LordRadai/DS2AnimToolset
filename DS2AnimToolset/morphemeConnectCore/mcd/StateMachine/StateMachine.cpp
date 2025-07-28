#include "StateMachine.h"

namespace mcd
{
	StateMachine::StateMachine(db::Attribute* parent, const std::string& name, const std::string& nodeType, int manifestVersion)
		: Graph(parent, "StateMachine", name),
		m_nodeType(std::make_unique<db::StringAttribute>(this, "NodeType", nodeType)),
		m_manifestVersion(std::make_unique<db::IntAttribute>(this, "ManifestVersion", manifestVersion)),
		m_stateMachineNodes(std::make_unique<db::TypedNodeContainer<StateMachineNode>>(this, "StateMachineNodes")),
		m_transitionEdges(std::make_unique<db::TypedNodeContainer<TransitionEdge>>(this, "TransitionEdges")),
		m_defaultState(std::make_unique<db::Pointer<StateMachineNode>>(this, "DefaultState", nullptr))
	{
		addAttribute(m_nodeType.get());
		addAttribute(m_manifestVersion.get());
		addAttribute(m_stateMachineNodes.get());
		addAttribute(m_transitionEdges.get());
		addAttribute(m_defaultState.get());
	}
}
