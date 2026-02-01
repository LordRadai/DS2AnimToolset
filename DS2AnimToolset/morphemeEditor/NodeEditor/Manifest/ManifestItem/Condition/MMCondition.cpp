#include "MMCondition.h"

namespace NodeEditor
{
	namespace Manifest
	{
		MMCondition::~MMCondition()
		{
			for (size_t i = 0; i < m_attributes.size(); i++)
				delete m_attributes[i];

			m_attributes.clear();
		}

		void MMCondition::fromJson(const nlohmann::json& json)
		{
			m_jsonData = json;

			if (json.contains("attributes"))
			{
				m_attributes.clear();
				for (const auto& attr : json["attributes"])
					m_attributes.push_back(new MMAttribute(attr));
			}
		}

		MMAttribute* MMCondition::getAttribute(uint32_t index)
		{
			if (index < m_attributes.size())
				return m_attributes[index];
		}

		MMAttribute* MMCondition::findAttribute(const std::string& name)
		{
			for (size_t i = 0; i < m_attributes.size(); i++)
			{
				if (m_attributes[i]->getName() == name)
					return m_attributes[i];
			}

			return nullptr;
		}
	}
}