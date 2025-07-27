#pragma once
#include "NMDatabase/NMDatabase.h"
#include "mcd/Graph/Graph.h"
#include "NMDBExtensions/TypedNodeContainer.inl"

namespace mcd
{
	class Network : public db::Node
	{
		db::TypedNodeContainer<mcd::Graph> m_graphs;
	};
}
