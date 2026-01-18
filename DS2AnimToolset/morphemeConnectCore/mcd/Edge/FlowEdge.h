#pragma once
#include "Edge.inl"
#include "NMDBExtensions/Pointer.inl"
#include "mcd/Pin/Pin.h"

namespace mcd
{
	class FlowEdge : public Edge
	{
		std::unique_ptr<db::Pointer<mcd::Pin>> m_from;
		std::unique_ptr<db::Pointer<mcd::Pin>> m_to;

	public:
		FlowEdge(db::CompositeAttribute* parent, std::string name, mcd::Pin* to, mcd::Pin* from);

		virtual ~FlowEdge() override {};

		mcd::Pin* getSrcPin() const { return m_from->getValue(); }
		mcd::Pin* getDstPin() const { return m_to->getValue(); }
	};
}
