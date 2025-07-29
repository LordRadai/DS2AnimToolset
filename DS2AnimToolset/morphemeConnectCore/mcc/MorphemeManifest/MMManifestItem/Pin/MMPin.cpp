#include "MMPin.h"

namespace mcc
{
	void MMPin::fromJson(const nlohmann::json& json)
	{
		this->m_jsonData = json;

		if (json.contains("name"))
			m_name = json["name"].get<std::string>();

		if (json.contains("displayName"))
			m_displayName = json["displayName"].get<std::string>();

		if (json.contains("input"))
			m_bIsInput = json["input"].get<bool>();

		if (json.contains("array"))
			m_bIsArray = json["array"].get<bool>();
	}
}
