#include "Graph.h"

namespace mcd
{
	Graph::Graph(Attribute* parent, std::string name, std::string graphName)
		: Node(parent, name, graphName),
		m_flowEdges(std::make_unique<db::TypedNodeContainer<mcd::FlowEdge>>(this, "FlowEdges")),
		m_panX(std::make_unique<db::FloatAttribute>(this, "PanX", 0.0f)),
		m_panY(std::make_unique<db::FloatAttribute>(this, "PanY", 0.0f))
	{
		addAttribute(m_flowEdges.get());
		addAttribute(m_panX.get());
		addAttribute(m_panY.get());
	}
}
