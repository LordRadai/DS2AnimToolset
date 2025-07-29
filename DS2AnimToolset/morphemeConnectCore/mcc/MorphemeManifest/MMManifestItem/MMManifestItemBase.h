#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace mcc
{
	class MMManifestItemBase
	{
	protected:
		nlohmann::json m_jsonData;
	public:
		MMManifestItemBase() {};

		virtual ~MMManifestItemBase() {};
		virtual nlohmann::json toJson() const { return nullptr; };
		virtual void fromJson(const nlohmann::json& json) = 0;
	};
}
