#pragma once
#include "MMManifestItemBase.h"

namespace mcc
{
	class MMAttribute : public MMManifestItemBase
	{
		std::string m_type;
		std::string m_name;
		void* m_value;
		std::string m_units;
		float m_min;
		float m_max;
		std::string m_displayName;
		std::string m_helpText;
		bool m_bPerAnimSet;

	public:
		MMAttribute() : m_value(nullptr), m_min(0.0f), m_max(0.0f), m_bPerAnimSet(false) {}

		virtual ~MMAttribute() override {}
		nlohmann::json toJson() const override;
		void fromJson(const nlohmann::json& json) override;
		std::string getType() const { return m_type; }
		std::string getName() const { return m_name; }

		bool getValueAsBool() const { return m_value ? *static_cast<bool*>(m_value) : false; }
		int getValueAsInt() const { return m_value ? *static_cast<int*>(m_value) : 0; }
		float getValueAsFloat() const { return m_value ? *static_cast<float*>(m_value) : 0.0f; }
		std::string getValueAsString() const { return m_value ? *static_cast<std::string*>(m_value) : ""; }
	};
}
