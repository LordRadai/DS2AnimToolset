#include "Graph.h"

namespace mcd
{
	Graph::Graph(Attribute* parent, std::string name)
		: Node(parent, name, "Graph"),
		m_flowEdges(this, "FlowEdges"),
		m_panX(this, "PanX", 0.0f),
		m_panY(this, "PanY", 0.0f)
	{
		addAttribute(&m_flowEdges);
		addAttribute(&m_panX);
		addAttribute(&m_panY);
	}
}
