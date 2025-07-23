#pragma once
#include "GraphNode/GraphNode.h"

namespace mcd
{
	class StateMachineNode : public GraphNode
	{
	public:
		StateMachineNode(db::Node* parent, std::string label, std::string type, uint32_t version) :
			GraphNode(parent, "StateMachineNode", label, type, version)
		{
		};

		virtual ~StateMachineNode() override {};
		virtual bool isValid() const override { return true; } // Placeholder for actual validation logic
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) override;
		
		// Additional methods specific to StateMachineNode can be added here
	};
}
