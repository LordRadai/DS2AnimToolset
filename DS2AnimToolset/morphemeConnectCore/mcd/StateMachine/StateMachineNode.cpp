#include "StateMachineNode.h"

namespace mcd
{
	StateMachineNode::StateMachineNode(db::CompositeAttribute* parent, std::string name, float xPos, float yPos, float width, float height, std::string nodeType, int manifestVersion)
		: GraphNode(parent, "StateMachineNode", name, xPos, yPos, width, height),
		m_manifestVersion(std::make_unique<db::IntAttribute>(this, "ManifestVersion", manifestVersion)),
		m_nodeType(std::make_unique<db::StringAttribute>(this, "NodeType", nodeType))
	{
		addAttribute(m_manifestVersion.get());
		addAttribute(m_nodeType.get());
	}

	void StateMachineNode::setManifestVersion(int version)
	{
		m_manifestVersion->setValue(version);
	}

	void StateMachineNode::setNodeType(const std::string& type)
	{
		m_nodeType->setValue(type);
	}
}
