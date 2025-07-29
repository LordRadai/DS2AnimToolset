#include "MMDataPin.h"

namespace mcc
{
	void MMDataPin::fromJson(const nlohmann::json& json)
	{
		MMPin::fromJson(json);

		if (!json.contains("type"))
			throw std::runtime_error("MMDataPin JSON does not contain 'type' field");

		m_dataType = getDataTypeFromString(json["type"].get<std::string>());
	}
}
