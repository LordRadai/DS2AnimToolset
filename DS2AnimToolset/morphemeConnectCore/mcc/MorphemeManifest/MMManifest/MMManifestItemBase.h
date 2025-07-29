#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace mcc
{
	class MMManifestItemBase
	{
	public:
		MMManifestItemBase() {};

		virtual ~MMManifestItemBase() {};
		virtual nlohmann::json toJson() const = 0;
		virtual void fromJson(const nlohmann::json& json) = 0;
	};
}
