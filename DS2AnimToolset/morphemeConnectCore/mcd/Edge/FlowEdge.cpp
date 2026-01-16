#include "FlowEdge.h"

namespace mcd
{
	FlowEdge::FlowEdge(CompositeAttribute* parent, std::string name, mcd::Pin* to, mcd::Pin* from)
		: Edge(parent, "FlowEdge", name), 
		m_from(std::make_unique<db::Pointer<mcd::Pin>>(this, "From", from)), 
		m_to(std::make_unique<db::Pointer<mcd::Pin>>(this, "To", to))
	{
		addAttribute(m_from.get());
		addAttribute(m_to.get());
	}
}
