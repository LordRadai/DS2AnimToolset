#pragma once
#include "../Node/MMNode.h"
#include "mcd/StateMachine/StateMachineNode.h"

namespace mcc
{
	class MMStateMachineNode : public MMNode
	{
	public:
		MMStateMachineNode() = default;
		MMStateMachineNode(const nlohmann::json& json) : MMNode(json) {};

		virtual ~MMStateMachineNode() override = default;

		mcd::StateMachineNode* createStateMachineNode(const std::string name);
	};
}
