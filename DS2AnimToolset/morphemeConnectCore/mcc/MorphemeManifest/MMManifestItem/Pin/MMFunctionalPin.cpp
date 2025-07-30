#include "MMFunctionalPin.h"
#include "mcd/Pin/FunctionalPin.h"

namespace mcc
{
	void MMFunctionalPin::fromJson(const nlohmann::json& json)
	{
		if (!json.contains("interfaces"))
			throw std::runtime_error("MMFunctionalPin JSON does not contain 'interfaces' field");

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

	bool MMFunctionalPin::addToGraphNode(mcd::GraphNode* node)
	{
		mcd::FunctionalPin* pin = new mcd::FunctionalPin(node, getPinName());
		pin->setPassThroughEnabled(isPassThrough());

		for (size_t i = 0; i < this->m_requiredInterfaces.size(); i++)
			pin->addInterface(this->m_requiredInterfaces[i]);

		for (size_t i = 0; i < this->m_optionalInterfaces.size(); i++)
			pin->addInterface(this->m_optionalInterfaces[i]);

		node->getPins()->add(pin);

		return true;
	}
}