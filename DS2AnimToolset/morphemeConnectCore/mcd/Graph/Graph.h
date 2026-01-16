#pragma once
#include "NMDatabase/NMDatabase.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "mcd/Edge/FlowEdge.h"
#include "GraphNode.h"

namespace mcd
{
	class Graph : public db::Node
	{
	protected:
		std::unique_ptr<db::TypedNodeContainer<mcd::FlowEdge>> m_flowEdges;
		std::unique_ptr<db::FloatAttribute> m_panX;
		std::unique_ptr<db::FloatAttribute> m_panY;

		Graph(CompositeAttribute* parent, std::string name, std::string graphName);
	public:
		virtual ~Graph() override {};
		virtual bool hasEdge(mcd::Edge* edge) const { return edge->getParentNode() == this; }
		virtual bool hasNode(mcd::GraphNode* node) const { return false; };

		mcd::FlowEdge* getFlowEdge(const uint32_t index) const { m_flowEdges->getNode(index); };

		float getPanX() const { return m_panX->getValue(); }
		float getPanY() const { return m_panY->getValue(); }
		void setPan(float x, float y);

		bool removeConnection(mcd::Edge* edge);
		bool removeConnection(mcd::Pin* pin);
	};
}
