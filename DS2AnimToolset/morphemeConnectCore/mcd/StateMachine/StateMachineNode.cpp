#include "StateMachineNode.h"

namespace mcd
{
	StateMachineNode::StateMachineNode(db::Attribute* parent, const std::string& name, float xpos, float ypos, float width, float height)
		: GraphNode(parent, "StateMachineNode", name, xpos, ypos, width, height),
		m_manifestVersion(std::make_unique<db::IntAttribute>(this, "ManifestVersion", 1)),
		m_nodeType(std::make_unique<db::StringAttribute>(this, "NodeType", ""))
	{
	}

	void StateMachineNode::setManifestVersion(int version)
	{
		m_manifestVersion->setValue(version);

		removeAttribute(m_manifestVersion.get());
		addAttribute(m_manifestVersion.get());
	}

	void StateMachineNode::setNodeType(const std::string& type)
	{
		m_nodeType->setValue(type);

		removeAttribute(m_manifestVersion.get());
		addAttribute(m_manifestVersion.get());
	}
}
