#pragma once
#include "MMPin.h"

namespace Manifest
{
	class MMFunctionalPin : public MMPin
	{
		std::vector<std::string> m_optionalInterfaces;
		std::vector<std::string> m_requiredInterfaces;

	public:
		MMFunctionalPin() {}
		MMFunctionalPin(const std::string& name, const nlohmann::json& json) : MMPin(name, json) { fromJson(json); };

		virtual ~MMFunctionalPin() override = default;
		virtual void fromJson(const nlohmann::json& json) override;

		bool isPassThrough() const { return m_jsonData.value("passThrough", false); }

		bool hasOptionalInterface(const std::string& interfaceName) const { return getOptionalInterface(interfaceName) != ""; }
		bool hasRequiredInterface(const std::string& interfaceName) const { return getRequiredInterface(interfaceName) != ""; }

		std::string getOptionalInterface(uint32_t index) const { return m_optionalInterfaces[index]; }
		std::string getOptionalInterface(const std::string& interfaceName) const;

		std::string getRequiredInterfaces(uint32_t index) const { return m_requiredInterfaces[index]; }
		std::string getRequiredInterface(const std::string& interfaceName) const;

		uint32_t getOptionalInterfaceCount() const { return static_cast<uint32_t>(m_optionalInterfaces.size()); }
		uint32_t getRequiredInterfaceCount() const { return static_cast<uint32_t>(m_requiredInterfaces.size()); }
	};
}
