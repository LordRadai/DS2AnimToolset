#pragma once
#include "NMDatabase/NMDatabase.h"
#include "mcd/Graph/Graph.h"
#include "mcd/ControlParametersNode/ControlParametersNode.h"

namespace mcd
{
	class Network : public db::Node
	{
		std::unique_ptr<db::TypedNodeContainer<mcd::Graph>> m_graphs;
		std::unique_ptr<mcd::ControlParametersNode> m_controlParametersNode;

	public:
		Network(db::Node* parent, const std::string& name);

		void reparentGraph(mcd::Graph* newParent) {};
	};
}
