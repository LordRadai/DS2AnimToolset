#pragma once
#include "MMPin.h"
#include "DataTypes.h"

namespace mcc
{
	class MMDataPin : public MMPin
	{
	public:
		MMDataPin() = default;
		MMDataPin(const nlohmann::json& json) { fromJson(json); }

		virtual ~MMDataPin() override = default;
		std::string getDataType() const { return m_jsonData["type"]; }
	};
}
