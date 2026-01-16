#include "ControlParametersNode.h"

namespace mcd
{
	ControlParametersNode::ControlParametersNode(db::CompositeAttribute* parent, std::string name)
		: Node(parent, "ControlParametersNode", name),
		m_controlParameters(std::make_unique<db::TypedNodeContainer<mcd::ControlParameter>>(this, "ControlParameterArray")),
		m_cpBlendTree(std::make_unique<mcd::BlendTree>(this, "ControlParameterBlendTree"))
	{
		addAttribute(m_controlParameters.get());
		addAttribute(m_cpBlendTree.get());
	}
}
