#pragma once
#include "MMNode.h"

namespace Manifest
{
	class MMStateMachine : public MMNode
	{
	public:
		MMStateMachine() : MMNode() {};
		MMStateMachine(const nlohmann::json& json) : MMNode(json) {};

		virtual ~MMStateMachine() override = default;
	};
}
