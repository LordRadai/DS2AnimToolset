#pragma once
#include "MMPin.h"
#include "DataTypes.h"
#include "mcd/Graph/GraphNode.h"

namespace mcc
{
	class MMDataPin : public MMPin
	{
	public:
		MMDataPin() = default;
		MMDataPin(const std::string& name, const nlohmann::json& json) : MMPin(name, json) {};

		virtual ~MMDataPin() override = default;
		std::string getDataType() const { return m_jsonData["type"]; }

		bool addToGraphNode(mcd::GraphNode* node);
	};
}
