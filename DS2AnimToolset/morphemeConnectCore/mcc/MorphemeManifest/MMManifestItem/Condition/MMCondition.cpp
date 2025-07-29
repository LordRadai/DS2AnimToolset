#include "MMCondition.h"

namespace mcc
{
	void MMCondition::fromJson(const nlohmann::json& json)
	{
		m_jsonData = json;

		if (json.contains("attributes"))
		{
			m_attributes.clear();
			for (const auto& attr : json["attributes"])
				m_attributes.push_back(MMAttribute(attr));
		}
	}
}
