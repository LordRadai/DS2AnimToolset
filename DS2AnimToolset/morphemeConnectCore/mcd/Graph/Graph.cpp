#include "Graph.h"

namespace mcd
{
	Graph::Graph(CompositeAttribute* parent, std::string name, std::string graphName)
		: Node(parent, name, graphName),
		m_flowEdges(std::make_unique<db::TypedNodeContainer<mcd::FlowEdge>>(this, "FlowEdges")),
		m_panX(std::make_unique<db::FloatAttribute>(this, "PanX", 0.0f)),
		m_panY(std::make_unique<db::FloatAttribute>(this, "PanY", 0.0f))
	{
	}

	void Graph::setPan(float x, float y)
	{
		removeAttribute(m_panX.get());
		removeAttribute(m_panY.get());

		m_panX->setValue(x);
		m_panY->setValue(y);

		if (x != 0.f)
			addAttribute(m_panX.get());

		if (y != 0.f)
			addAttribute(m_panY.get());
	}

	bool Graph::removeConnection(mcd::Edge* edge)
	{
		throw std::runtime_error("Graph::removeConnection(Edge*) not implemented");
	}

	bool Graph::removeConnection(mcd::Pin* pin)
	{
		throw std::runtime_error("Graph::removeConnection(Pin*) not implemented");
	}
}
