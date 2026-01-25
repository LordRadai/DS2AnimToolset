#include "EmittedControlParametersNode.h"

namespace mcd
{
	EmittedControlParametersNode::EmittedControlParametersNode(db::CompositeAttribute* parent, const std::string& name)
		: Node(parent, "EmittedControlParametersNode", name),
		m_emittedControlParameters(std::make_unique<db::TypedNodeContainer<EmittedControlParameter>>(this, "EmittedControlParameterArray"))
	{
		addAttribute(m_emittedControlParameters.get());
	}
}
