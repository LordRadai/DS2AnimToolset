#pragma once
#include "MMNode.h"
#include "NodeEditor/Editor/Node/StateNode.h"

namespace NodeEditor
{
	namespace Manifest
	{
		class MMStateMachineNode : public MMNode
		{
		public:
			MMStateMachineNode() = default;
			MMStateMachineNode(const nlohmann::json& json) : MMNode(json) {};

			virtual ~MMStateMachineNode() override = default;

			StateNode* makeNode(Editor* editor, Graph* parent, int id, const std::string& name);
		};
	}
}