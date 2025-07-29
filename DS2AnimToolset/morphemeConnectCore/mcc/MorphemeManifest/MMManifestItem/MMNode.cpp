#include "MMNode.h"

namespace mcc
{
	void MMNode::fromJson(const nlohmann::json& json)
	{
		this->m_jsonData = json;

		if (json.contains("attributes"))
		{
			for (const auto& attrJson : json["attributes"])
				m_attributes.push_back(MMAttribute(attrJson));
		}

		if (json.contains("dataPins"))
		{
			for (const auto& pinJson : json["dataPins"])
				m_dataPins.push_back(MMDataPin(json));
		}

		if (json.contains("functionPins"))
		{
			for (const auto& pinJson : json["functionPins"])
				m_functionalPins.push_back(MMFunctionalPin(pinJson));
		}
	}

	MMAttribute* MMNode::getAttribute(uint32_t index)
	{
		if (index >= m_attributes.size())
			return nullptr;

		return &m_attributes[index];
	}

	MMAttribute* MMNode::findAttribute(const std::string& name)
	{
		for (auto& attr : m_attributes)
		{
			if (attr.getName() == name)
				return &attr;
		}

		return nullptr;
	}

	MMDataPin* MMNode::getDataPin(uint32_t index)
	{
		if (index >= m_dataPins.size())
			return nullptr;

		return &m_dataPins[index];
	}

	MMDataPin* MMNode::findDataPin(const std::string& name)
	{
		for (auto& pin : m_dataPins)
		{
			if (pin.getName() == name)
				return &pin;
		}

		return nullptr;
	}

	MMFunctionalPin* MMNode::getFunctionalPin(uint32_t index)
	{
		if (index >= m_functionalPins.size())
			return nullptr;

		return &m_functionalPins[index];
	}

	MMFunctionalPin* MMNode::findFunctionalPin(const std::string& name)
	{
		for (auto& pin : m_functionalPins)
		{
			if (pin.getName() == name)
				return &pin;
		}

		return nullptr;
	}
}
