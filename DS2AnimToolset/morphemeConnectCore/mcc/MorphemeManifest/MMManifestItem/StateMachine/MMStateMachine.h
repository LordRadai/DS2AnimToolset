#pragma once
#include "../Node/MMNode.h"

namespace mcc
{
	class MMStateMachine : public MMNode
	{
	public:
		MMStateMachine() : MMNode() {};
		MMStateMachine(const nlohmann::json& json) : MMNode(json) {};

		virtual ~MMStateMachine() override = default;
	};
}
