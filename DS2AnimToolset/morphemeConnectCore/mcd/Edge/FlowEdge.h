#pragma once
#include "Edge.inl"
#include "NMDBExtensions/Pointer.inl"
#include "mcd/Pin/Pin.inl"

namespace mcd
{
	class FlowEdge : public Edge
	{
		db::Pointer<mcd::Pin> m_from;
		db::Pointer<mcd::Pin> m_to;

	public:
		FlowEdge(Attribute* parent, std::string name, mcd::Pin* to, mcd::Pin* from);

		virtual ~FlowEdge() override {};
	};
}
