#pragma once
#include "../MMManifestItemBase.h"

namespace mcc
{
	class MMPin : public MMManifestItemBase
	{
	public:
		MMPin() = default;
		MMPin(const nlohmann::json& json) { fromJson(json); }

		virtual ~MMPin() override = default;
		virtual void fromJson(const nlohmann::json& json) override;

		std::string getPinName() const { return m_jsonData["name"]; }
		std::string getPinType() const { return m_jsonData["type"]; }
		std::string getDisplayName() const { return m_jsonData["displayName"]; }
		bool isInput() const { return m_jsonData["input"]; }
		bool isArray() const { return m_jsonData["array"]; }
	};
}
