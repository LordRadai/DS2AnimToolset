#pragma once
#include "NMDatabase/NMDatabase.h"
#include "mcd/AttributePinNodeBase/AttributePinNodeBase.h"

namespace mcd
{
	class Edge : public mcd::AttributePinNodeBase
	{
	protected:
		Edge(db::CompositeAttribute* parent, std::string name, std::string nodeName) : mcd::AttributePinNodeBase(parent, name, nodeName) {}

	public:
		virtual ~Edge() {};
	};
}
