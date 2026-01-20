#include "BlendTreeNode.h"
#include "mcd/Pin/DataPin.h"

namespace mcd
{
	BlendTreeNode::BlendTreeNode(db::CompositeAttribute* parent, std::string name, float xPos, float yPos, float width, float height, std::string nodeType, int manifestVersion)
		: GraphNode(parent, "BlendTreeNode", name, xPos, yPos, width, height),
		m_nodeType(std::make_unique<db::StringAttribute>(this, "NodeType", nodeType)),
		m_manifestVersion(std::make_unique<db::IntAttribute>(this, "ManifestVersion", manifestVersion))
	{
		addAttribute(m_nodeType.get());
		addAttribute(m_manifestVersion.get());
	}

	bool BlendTreeNode::isOperatorNode()
	{
		const int pinCount = getPinCount();

		if (pinCount == 0)
			return false;

		for (size_t i = 0; i < pinCount; i++)
		{
			Pin* pin = getPin(static_cast<int>(i));

			if (!pin->isOfType<DataPin>())
				return false;
		}

		return true;
	}
}
