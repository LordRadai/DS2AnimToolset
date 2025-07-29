#pragma once
#include "Attribute/MMAttribute.h"
#include "Pin/MMDataPin.h"
#include "Pin/MMFunctionalPin.h"

namespace mcc
{
	class MMNode : public MMManifestItemBase
	{
		std::string m_group;
		std::string m_displayName;
		int m_id;
		std::string m_image;
		int m_version;
		std::vector<MMAttribute> m_attributes;
		std::vector<MMDataPin> m_dataPins;
		std::vector<MMFunctionalPin> m_functionalPins;

	public:
		MMNode() : m_id(-1), m_version(0) {}

		virtual ~MMNode() override = default;
		virtual void fromJson(const nlohmann::json& json) override;
	};
}
