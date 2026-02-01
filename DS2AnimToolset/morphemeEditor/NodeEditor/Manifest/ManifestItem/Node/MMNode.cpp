#include "MMNode.h"

namespace Manifest
{
	MMNode::~MMNode()
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
			delete m_attributes[i];

		for (size_t i = 0; i < m_dataPins.size(); i++)
			delete m_dataPins[i];

		for (size_t i = 0; i < m_functionalPins.size(); i++)
			delete m_functionalPins[i];

		m_attributes.clear();
		m_dataPins.clear();
		m_functionalPins.clear();
		m_pinOrder.clear();
	}

	void MMNode::fromJson(const nlohmann::json& json)
	{
		this->m_jsonData = json;

		if (json.contains("attributes"))
		{
			for (const auto& attrJson : json["attributes"])
				m_attributes.push_back(new MMAttribute(attrJson));
		}

		if (json.contains("dataPins"))
		{
			for (const auto& [name, pinJson] : json["dataPins"].items())
				m_dataPins.push_back(new MMDataPin(name, pinJson));
		}

		if (json.contains("functionPins"))
		{
			for (const auto& [name, pinJson] : json["functionPins"].items())
				m_functionalPins.push_back(new MMFunctionalPin(name, pinJson));
		}

		if (json.contains("pinOrder"))
		{
			for (const auto& pinName : json["pinOrder"])
			{
				if (pinName.is_string())
					m_pinOrder.push_back(pinName.get<std::string>());
			}
		}
	}

	MMAttribute* MMNode::getAttribute(uint32_t index)
	{
		if (index >= m_attributes.size())
			return nullptr;

		return m_attributes[index];
	}

	MMAttribute* MMNode::findAttribute(const std::string& name)
	{
		for (auto& attr : m_attributes)
		{
			if (attr->getName() == name)
				return attr;
		}

		return nullptr;
	}

	MMDataPin* MMNode::getDataPin(uint32_t index)
	{
		if (index >= m_dataPins.size())
			return nullptr;

		return m_dataPins[index];
	}

	MMDataPin* MMNode::findDataPin(const std::string& name)
	{
		for (auto& pin : m_dataPins)
		{
			if (pin->getPinName() == name)
				return pin;
		}

		return nullptr;
	}

	MMFunctionalPin* MMNode::getFunctionalPin(uint32_t index)
	{
		if (index >= m_functionalPins.size())
			return nullptr;

		return m_functionalPins[index];
	}

	MMFunctionalPin* MMNode::findFunctionalPin(const std::string& name)
	{
		for (auto& pin : m_functionalPins)
		{
			if (pin->getPinName() == name)
				return pin;
		}

		return nullptr;
	}

	std::vector<MMPin*> MMNode::getSortedPins()
	{
		std::vector<MMPin*> sortedPins;
		sortedPins.reserve(m_functionalPins.size() + m_dataPins.size());

		for (const auto& pinName : m_pinOrder)
		{
			MMPin* pin = findFunctionalPin(pinName);

			if (pin == nullptr)
				pin = findDataPin(pinName);

			if (pin)
				sortedPins.push_back(pin);
		}

		return sortedPins;
	}
}
