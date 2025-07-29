#include "MMCondition.h"

namespace mcc
{
	void MMCondition::fromJson(const nlohmann::json& json)
	{
		m_jsonData = json;

		if (json.contains("attributes") && json["attributes"].is_array())
		{
			m_attributes.clear();
			for (const auto& attr : json["attributes"])
			{
				if (attr.is_object())
					m_attributes.push_back(MMAttribute(attr));
			}
		}
	}
}
