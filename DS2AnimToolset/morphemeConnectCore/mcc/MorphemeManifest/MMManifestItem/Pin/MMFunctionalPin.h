#pragma once
#include "MMPin.h"

namespace mcc
{
	class MMFunctionalPin : public MMPin
	{
		std::vector<std::string> m_optionalInterfaces;
		std::vector<std::string> m_requiredInterfaces;
		bool m_passThroughEnabled;

	public:
		MMFunctionalPin() : m_passThroughEnabled(false) {}

		virtual ~MMFunctionalPin() override = default;
		virtual void fromJson(const nlohmann::json& json) override;

		void addOptionalInterface(const std::string& interfaceName) { m_optionalInterfaces.push_back(interfaceName); }
		void addRequiredInterface(const std::string& interfaceName) { m_requiredInterfaces.push_back(interfaceName); }
		bool isPassThroughEnabled() const { return m_passThroughEnabled; }
		void setPassThroughEnabled(bool enabled) { m_passThroughEnabled = enabled; }

		std::string getOptionalInterface(uint32_t index) const { return m_optionalInterfaces[index]; }
		std::string getRequiredInterfaces(uint32_t index) const { return m_requiredInterfaces[index]; }
		uint32_t getOptionalInterfaceCount() const { return static_cast<uint32_t>(m_optionalInterfaces.size()); }
		uint32_t getRequiredInterfaceCount() const { return static_cast<uint32_t>(m_requiredInterfaces.size()); }
	};
}
