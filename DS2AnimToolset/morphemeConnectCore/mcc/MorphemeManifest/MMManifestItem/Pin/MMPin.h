#pragma once
#include "../MMManifestItemBase.h"

namespace mcc
{
	class MMPin : public MMManifestItemBase
	{
	protected:
		std::string m_name;
		std::string m_pinType;
		std::string m_displayName;
		bool m_bIsInput = false;
		bool m_bIsArray = false;

	public:
		MMPin() = default;

		virtual ~MMPin() override = default;
		virtual void fromJson(const nlohmann::json& json) override;

		std::string getName() const { return m_name; }
		std::string getPinType() const { return m_pinType; }
		std::string getDisplayName() const { return m_displayName; }
		bool isInput() const { return m_bIsInput; }
		bool isArray() const { return m_bIsArray; }
	};
}
