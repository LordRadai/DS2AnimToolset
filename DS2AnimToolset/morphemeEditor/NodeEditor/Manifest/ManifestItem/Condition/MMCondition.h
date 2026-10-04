#pragma once
#include "../ManifestItemBase.h"
#include "../Attribute/MMAttribute.h"
#include "NodeEditor/Editor/Condition/Condition.h"

namespace NodeEditor
{
	namespace Manifest
	{
		class MMCondition : public ManifestItemBase
		{
			std::vector<MMAttribute*> m_attributes;
		public:
			MMCondition() = default;
			MMCondition(const nlohmann::json& json) { fromJson(json); }

			virtual ~MMCondition() override;
			virtual void fromJson(const nlohmann::json& json) override;

			int getID() const { return m_jsonData["id"]; }

			void addAttribute(MMAttribute* attribute) { m_attributes.push_back(attribute); }
			MMAttribute* getAttribute(uint32_t index);
			MMAttribute* findAttribute(const std::string& name);
			uint32_t getNumAttributes() const { return static_cast<uint32_t>(m_attributes.size()); }

			Condition* makeCondition(Transition* owner);
		};
	}
}