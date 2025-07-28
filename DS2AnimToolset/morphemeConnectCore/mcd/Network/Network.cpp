#include "Network.h"

namespace mcd
{
	Network::Network(db::Node* parent, const std::string& name)
		: db::Node(parent, "Network", name),
		m_graphs(std::make_unique<db::TypedNodeContainer<mcd::Graph>>(this, "GraphEntry")),
		m_controlParametersNode(std::make_unique<mcd::ControlParametersNode>(this, "ControlParametersNode"))
	{
		addAttribute(m_graphs.get());
		addAttribute(m_controlParametersNode.get());
	}
}
