#include "MMCondition.h"
#include "mcc/AttributeInfo/AttributeInfo.h"

namespace mcc
{
	MMCondition::~MMCondition()
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
			delete m_attributes[i];

		m_attributes.clear();
	}

	void MMCondition::fromJson(const nlohmann::json& json)
	{
		m_jsonData = json;

		if (json.contains("attributes"))
		{
			m_attributes.clear();
			for (const auto& attr : json["attributes"])
				m_attributes.push_back(new MMAttribute(attr));
		}
	}

	MMAttribute* MMCondition::getAttribute(uint32_t index)
	{
		if (index < m_attributes.size())
			return m_attributes[index];
	}

	MMAttribute* MMCondition::findAttribute(const std::string& name)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			if (m_attributes[i]->getName() == name)
				return m_attributes[i];
		}

		return nullptr;
	}

	mcd::Condition* MMCondition::createDatabaseCondition(mcd::TransitionEdge* parent)
	{
		std::string conditionType = getName();

		char conditionName[256];
		snprintf(conditionName, sizeof(conditionName), "%s%d", conditionType.c_str(), parent->getNumConditionsOfType(getName()) + 1);

		mcd::Condition* condition = new mcd::Condition(parent, conditionName, conditionType, getVersion());

		for (size_t i = 0; i < this->m_attributes.size(); i++)
		{
			mcc::AttributeInfo attrInfo(this->m_attributes[i]);

			condition->addAttribute(attrInfo.createNormalDatabaseAttribute(condition->getAttributes()));
		}

		parent->addCondition(condition);

		return condition;
	}
}
