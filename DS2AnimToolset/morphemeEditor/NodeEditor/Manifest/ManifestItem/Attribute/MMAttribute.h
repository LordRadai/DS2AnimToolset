#pragma once
#include "../ManifestItemBase.h"
#include "NodeEditor/Editor/Attribute/Attribute.h"

namespace NodeEditor
{
	namespace Manifest
	{
		class MMAttribute : public ManifestItemBase
		{
			std::string m_name;
		public:
			MMAttribute() {}
			MMAttribute(const nlohmann::json& json, const std::string& name = "");

			virtual ~MMAttribute() override {}
			virtual void fromJson(const nlohmann::json& json) override;

			std::string getName() const { return m_name; }
			std::string getDisplayName() const { return m_jsonData.value("displayName", m_name); }
			std::string getType() const { return m_jsonData["type"]; }
			std::vector<std::string> getEnumOptions() const;
			std::string getHelpText() const { return m_jsonData.value("helptext", ""); }
			nlohmann::json getValue() const { return m_jsonData.value("value", nlohmann::json(nullptr)); }
			bool isPerAnimSet() const { return m_jsonData.value("perAnimSet", false); }
			bool isSyncWithRigChannels() const { return m_jsonData.value("syncWithRigChannels", false); }
			bool isWeakRef() const { return m_jsonData.value("weak", false); }
			bool isAffectPins() const { return m_jsonData.value("affectPins", false); }
			std::string getRefKind() const { return m_jsonData.value("kind", ""); }

			bool isArray() const;
			uint32_t size() const;

			bool getBoolValue() const;
			float getFloatValue() const;
			int getIntValue() const;
			std::string getStringValue() const;

			bool getBoolValue(const uint32_t index) const;
			float getFloatValue(const uint32_t index) const;
			int getIntValue(const uint32_t index) const;
			std::string getStringValue(const uint32_t index) const;

			float getFloatMin() const { return m_jsonData.value("min", 0.0f); }
			float getFloatMax() const { return m_jsonData.value("max", 1.0f); }
			int getIntMin() const { return m_jsonData.value("min", -1); }
			int getIntMax() const { return m_jsonData.value("max", 1); }

			Attribute* makeAttribute(Entity* owner);
		};
	}
}