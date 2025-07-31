#include "TransitionEdge.h"

namespace mcd
{
	TransitionEdge::TransitionEdge(db::Node* parent, const std::string& name, mcd::StateMachineNode* sourceNode, mcd::StateMachineNode* targetNode, const std::string& transitType, const int manifestVersion)
		: db::Node(parent, "TransitionEdge", name),
		m_conditions(std::make_unique<db::TypedNodeContainer<mcd::Condition>>(this, "Conditions")),
		m_attributes(std::make_unique<db::TypedNodeContainer<mcd::Attribute>>(this, "Attributes")),
		m_sourceNode(std::make_unique<db::Pointer<mcd::StateMachineNode>>(this, "SourceNode", sourceNode)),
		m_targetNode(std::make_unique<db::Pointer<mcd::StateMachineNode>>(this, "TargetNode", targetNode)),
		m_edgeType(std::make_unique<db::StringAttribute>(this, "EdgeType", transitType)),
		m_manifestVersion(std::make_unique<db::IntAttribute>(this, "ManifestVersion", manifestVersion))
	{
		db::Node::addAttribute(m_conditions.get());
		db::Node::addAttribute(m_attributes.get());
		db::Node::addAttribute(m_sourceNode.get());
		db::Node::addAttribute(m_targetNode.get());
		db::Node::addAttribute(m_edgeType.get());
		db::Node::addAttribute(m_manifestVersion.get());
	}

	uint32_t TransitionEdge::getNumConditionsOfType(std::string type) const
	{
		uint32_t count = 0;

		for (uint32_t i = 0; i < m_conditions->size(); ++i)
		{
			mcd::Condition* condition = m_conditions->getNode(i);
			if (condition && condition->getType() == type)
				count++;
		}

		return count;
	}
}
