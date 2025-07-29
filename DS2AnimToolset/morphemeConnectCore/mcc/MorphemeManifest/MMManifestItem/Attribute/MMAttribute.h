#pragma once
#include "../MMManifestItemBase.h"

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
	};
}
