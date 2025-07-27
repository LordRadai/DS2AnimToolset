#include "FlowEdge.h"

namespace mcd
{
	FlowEdge::FlowEdge(Attribute* parent, std::string name, mcd::Pin* to, mcd::Pin* from)
		: Edge(parent, "FlowEdge", name), m_from(this, "From", from), m_to(this, "To", to)
	{
		addAttribute(&m_from);
		addAttribute(&m_to);
	}

	FlowEdge::~FlowEdge()
	{
	}
}
