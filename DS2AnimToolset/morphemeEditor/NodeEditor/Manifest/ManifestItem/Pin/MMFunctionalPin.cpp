#include "MMFunctionalPin.h"

namespace NodeEditor
{
	namespace Manifest
	{
		void MMFunctionalPin::fromJson(const nlohmann::json& json)
		{
			if (!json.contains("interfaces"))
				INVOKE_PANIC("MMFunctionalPin JSON does not contain 'interfaces' field");

			for (const auto& intr : json["interfaces"]["optional"])
				m_optionalInterfaces.push_back(intr.get<std::string>());

			for (const auto& intr : json["interfaces"]["required"])
				m_requiredInterfaces.push_back(intr.get<std::string>());
		}

		std::string MMFunctionalPin::getOptionalInterface(const std::string& interfaceName) const
		{
			for (const auto& intr : m_optionalInterfaces)
			{
				if (intr == interfaceName)
					return intr;
			}

			return "";
		}

		std::string MMFunctionalPin::getRequiredInterface(const std::string& interfaceName) const
		{
			for (const auto& intr : m_requiredInterfaces)
			{
				if (intr == interfaceName)
					return intr;
			}

			return "";
		}
	}
}