#pragma once
#include "MMPin.h"

namespace mcc
{
	class MMFunctionalPin : public MMPin
	{
		std::vector<std::string> m_optionalInterfaces;
		std::vector<std::string> m_requiredInterfaces;

	public:
		MMFunctionalPin() {}
		MMFunctionalPin(const nlohmann::json& json) { fromJson(json); }

		virtual ~MMFunctionalPin() override = default;
		virtual void fromJson(const nlohmann::json& json) override;

		bool isPassThroughEnabled() const { return false; }

		std::string getOptionalInterface(uint32_t index) const { return m_optionalInterfaces[index]; }
		std::string getRequiredInterfaces(uint32_t index) const { return m_requiredInterfaces[index]; }
		uint32_t getOptionalInterfaceCount() const { return static_cast<uint32_t>(m_optionalInterfaces.size()); }
		uint32_t getRequiredInterfaceCount() const { return static_cast<uint32_t>(m_requiredInterfaces.size()); }
	};
}
