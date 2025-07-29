#pragma once
#include "../MMManifestItemBase.h"
#include "../Attribute/MMAttribute.h"

namespace mcc
{
	class MMCondition : public MMManifestItemBase
	{
		std::vector<MMAttribute> m_attributes;
	public:
		MMCondition() = default;
		MMCondition(const nlohmann::json& json) { fromJson(json); }

		virtual ~MMCondition() override = default;
		virtual void fromJson(const nlohmann::json& json) override;

		int getID() const { return m_jsonData["id"]; }

		void addAttribute(const MMAttribute& attribute) { m_attributes.push_back(attribute); }
		MMAttribute* getAttribute(uint32_t index);
		MMAttribute* findAttribute(const std::string& name);
		uint32_t getNumAttributes() const { return static_cast<uint32_t>(m_attributes.size()); }
	};
}
