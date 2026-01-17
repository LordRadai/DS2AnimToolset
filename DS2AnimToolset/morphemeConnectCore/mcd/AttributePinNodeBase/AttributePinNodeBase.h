#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class AttributePinNodeBase : public db::Node
	{
	protected:
	public:
		AttributePinNodeBase(db::CompositeAttribute* parent, const std::string& name, const std::string nodeName)
			: db::Node(parent, name, nodeName) {};
		
		virtual ~AttributePinNodeBase() override {};

		virtual std::vector<db::Node> getDirectlyConnectedNodes(bool param_1, bool param_2);
	};
}
