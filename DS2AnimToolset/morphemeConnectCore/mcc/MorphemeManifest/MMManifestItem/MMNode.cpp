#include "MMNode.h"

namespace mcc
{
	void MMNode::fromJson(const nlohmann::json& json)
	{
		this->m_jsonData = json;

		if (!json.contains("group"))
			throw std::runtime_error("MMNode JSON does not contain 'group' field");

		m_group = json["group"].get<std::string>();

		if (json.contains("displayName"))
			m_displayName = json["displayName"].get<std::string>();

		if (!json.contains("id"))
			throw std::runtime_error("MMNode JSON does not contain 'id' field");

		m_id = json["id"].get<int>();

		if (json.contains("image"))
			m_image = json["image"].get<std::string>();

		if (!json.contains("version"))
			throw std::runtime_error("MMNode JSON does not contain 'version' field");

		m_version = json["version"].get<int>();

		if (json.contains("attributes"))
		{
			for (const auto& attrJson : json["attributes"])
			{
				MMAttribute attr;
				attr.fromJson(attrJson);
				m_attributes.push_back(attr);
			}
		}

		if (json.contains("dataPins"))
		{
			for (const auto& pinJson : json["dataPins"])
			{
				MMDataPin dataPin;
				dataPin.fromJson(pinJson);
				m_dataPins.push_back(dataPin);
			}
		}

		if (json.contains("functionPins"))
		{
			for (const auto& pinJson : json["functionPins"])
			{
				MMFunctionalPin functionalPin;
				functionalPin.fromJson(pinJson);
				m_functionalPins.push_back(functionalPin);
			}
		}
	}
}
