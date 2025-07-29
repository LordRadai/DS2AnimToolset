#include "MMFunctionalPin.h"

namespace mcc
{
	void MMFunctionalPin::fromJson(const nlohmann::json& json)
	{
		MMPin::fromJson(json);

		if (!json.contains("interfaces"))
			throw std::runtime_error("MMFunctionalPin JSON does not contain 'interfaces' field");

		for (const auto& interface : json["interfaces"]["optional"])
			m_optionalInterfaces.push_back(interface.get<std::string>());

		for (const auto& interface : json["interfaces"]["required"])
			m_requiredInterfaces.push_back(interface.get<std::string>());
	}
}