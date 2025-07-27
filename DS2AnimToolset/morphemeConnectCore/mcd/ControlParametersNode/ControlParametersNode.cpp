#include "ControlParametersNode.h"

namespace mcd
{
	ControlParametersNode::ControlParametersNode(db::Attribute* parent, std::string name)
		: Node(parent, "ControlParametersNode", name),
		m_controlParameters(this, "ControlParameterArray"),
		m_cpBlendTree(this, "ControlParameterBlendTree")
	{
		addAttribute(&m_controlParameters);
		addAttribute(&m_cpBlendTree);
	}
}
