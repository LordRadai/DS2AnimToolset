#pragma once
#include "MMNode.h"

namespace Manifest
{
	class MMStateMachineNode : public MMNode
	{
	public:
		MMStateMachineNode() = default;
		MMStateMachineNode(const nlohmann::json& json) : MMNode(json) {};

		virtual ~MMStateMachineNode() override = default;
	};
}
