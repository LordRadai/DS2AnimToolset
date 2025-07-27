#pragma once
#include "NMDatabase/NMDatabase.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "mcd/Edge/FlowEdge.h"

namespace mcd
{
	class Graph : public db::Node
	{
		db::TypedNodeContainer<mcd::FlowEdge> m_flowEdges;
		db::FloatAttribute m_panX;
		db::FloatAttribute m_panY;

	public:
		Graph(Attribute* parent, std::string name);

		virtual ~Graph() override {};

		mcd::FlowEdge* getFlowEdge(const uint32_t index) const { m_flowEdges.getNode(index); };

		float getPanX() const { return m_panX.getValue(); }
		float getPanY() const { return m_panY.getValue(); }
		void setPan(float x, float y) { m_panX.setValue(x); m_panY.setValue(y); }
	};
}
