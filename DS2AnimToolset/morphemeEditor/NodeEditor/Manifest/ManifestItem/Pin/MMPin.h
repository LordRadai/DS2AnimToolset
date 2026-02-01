#pragma once
#include "../ManifestItemBase.h"

namespace Manifest
{
	class MMPin : public ManifestItemBase
	{
	protected:
		std::string m_pinName;
	public:
		MMPin() = default;
		MMPin(const std::string& name, const nlohmann::json& json) : m_pinName(name) { fromJson(json); }

		virtual ~MMPin() override = default;
		virtual void fromJson(const nlohmann::json& json) override;

		std::string getPinName() const { return m_pinName; }
		std::string getPinType() const { return m_jsonData.value("type", ""); }
		std::string getDisplayName() const { return m_jsonData.value("displayName", ""); }
		bool isInput() const { return m_jsonData.value("input", false); }
		bool isArray() const { return m_jsonData.value("array", false); }
	};
}