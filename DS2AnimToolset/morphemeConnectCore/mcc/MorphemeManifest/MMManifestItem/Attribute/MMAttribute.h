#pragma once
#include "../MMManifestItemBase.h"
#include "mcd/Attribute/Attribute.h"

namespace mcc
{
	class MMAttribute : public MMManifestItemBase
	{
	public:
		MMAttribute() {}
		MMAttribute(const nlohmann::json& json) { fromJson(json); }

		virtual ~MMAttribute() override {}
		virtual void fromJson(const nlohmann::json& json) override;

		std::string getName() const { return m_jsonData["name"]; }
		std::string getType() const { return m_jsonData["type"]; }
		nlohmann::json getValue() const { return m_jsonData["value"]; }

		bool getBoolValue() const { return getValue().get<bool>(); }
		float getFloatValue() const { return getValue().get<float>(); }
		int getIntValue() const { return getValue().get<int>(); }
		std::string getStringValue() const { return getValue().get<std::string>(); }
	};
}
