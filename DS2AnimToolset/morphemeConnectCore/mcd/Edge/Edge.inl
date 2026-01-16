#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class Edge : public db::Node
	{
	protected:
		Edge(db::CompositeAttribute* parent, std::string name, std::string nodeName) : db::Node(parent, name, nodeName) {}

	public:
		virtual ~Edge() {};
	};
}
