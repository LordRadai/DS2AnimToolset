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

		std::string getPinName() const { return m_jsonData.value("name", ""); }
		std::string getPinType() const { return m_jsonData.value("type", ""); }
		std::string getDisplayName() const { return m_jsonData.value("displayName", ""); }
		bool isPassThrough() const { return m_jsonData.value("passThrough", false); }
		bool isInput() const { return m_jsonData.value("input", false); }
		bool isArray() const { return m_jsonData.value("array", false); }
	};
}
