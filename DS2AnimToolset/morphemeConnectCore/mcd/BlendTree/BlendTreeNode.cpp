#include "BlendTreeNode.h"

namespace mcd
{
	BlendTreeNode::BlendTreeNode(db::Attribute* parent, std::string name, float xPos, float yPos, float width, float height, std::string& nodeType, int manifestVersion)
		: GraphNode(parent, "BlendTreeNode", name, xPos, yPos, width, height),
		m_nodeType(std::make_unique<mcd::StringAttribute>(this, "NodeType", nodeType)),
		m_manifestVersion(std::make_unique<mcd::IntAttribute>(this, "ManifestVersion", manifestVersion))
	{
		addAttribute(m_nodeType.get());
		addAttribute(m_manifestVersion.get());
	}
}
