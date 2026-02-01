#include "MMTransition.h"

namespace NodeEditor
{
	namespace Manifest
	{
		void MMTransition::fromJson(const nlohmann::json& json)
		{
			m_jsonData = json;

			if (json.contains("interfaces") && json["interfaces"].is_array())
			{
				m_interfaces.clear();
				for (const auto& intr : json["interfaces"])
				{
					if (intr.is_string())
						m_interfaces.push_back(intr.get<std::string>());
				}
			}
		}

		std::string MMTransition::getInterface(uint32_t index) const
		{
			if (index < m_interfaces.size())
				return m_interfaces[index];

			throw std::out_of_range("Index out of range for interfaces.");
		}

		std::string MMTransition::findInterface(const std::string& name) const
		{
			for (const auto& intr : m_interfaces)
			{
				if (intr == name)
					return intr;
			}

			return "";
		}
	}
}