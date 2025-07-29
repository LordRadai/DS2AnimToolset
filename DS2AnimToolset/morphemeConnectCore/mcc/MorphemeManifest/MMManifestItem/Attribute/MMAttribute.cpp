#include "MMAttribute.h"

namespace mcc
{
	void MMAttribute::fromJson(const nlohmann::json& json)
	{
		this->m_jsonData = json;

		if (!json.contains("type"))
			throw std::runtime_error("MMAttribute JSON does not contain 'type' field.");

		m_type = getAttributeTypeFromString(json["type"].get<std::string>());

		if (!json.contains("name"))
			throw std::runtime_error("MMAttribute JSON does not contain 'name' field.");

		m_name = json["name"].get<std::string>();

		if (!json.contains("value"))
			throw std::runtime_error("MMAttribute JSON does not contain 'value' field.");

		m_value = json["value"];

		if (json.contains("units"))
			m_units = json["units"].get<std::string>();

		if (json.contains("min"))
			m_min = json["min"].get<float>();

		if (json.contains("max"))
			m_max = json["max"].get<float>();

		if (json.contains("displayName"))
			m_displayName = json["displayName"].get<std::string>();

		if (json.contains("helptext"))
			m_helpText = json["helptext"].get<std::string>();

		if (json.contains("perAnimSet"))
			m_bPerAnimSet = json["perAnimSet"].get<bool>();
	}
}