#include "BlendTreeNode.h"
#include "mcd/Pin/FunctionalPin.h"

namespace mcd
{
	BlendTreeNode::BlendTreeNode(db::CompositeAttribute* parent, std::string name, float xPos, float yPos, float width, float height, std::string nodeType, int manifestVersion)
		: GraphNode(parent, "BlendTreeNode", name, xPos, yPos, width, height),
		m_nodeType(std::make_unique<db::StringAttribute>(this, "NodeType", nodeType)),
		m_manifestVersion(std::make_unique<db::IntAttribute>(this, "ManifestVersion", manifestVersion))
	{
		addAttribute(m_nodeType.get());

		addPin(new mcd::FunctionalPin(this, "Result"));
	}
}
