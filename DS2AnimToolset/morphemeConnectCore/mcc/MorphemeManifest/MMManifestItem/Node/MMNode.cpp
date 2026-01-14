#include "MMNode.h"
#include "mcc/AttributeInfo/AttributeInfo.h"
#include "mcd/MorphemeDB/MorphemeDB.h"
#include "mcd/BlendTree/BlendTree.h"

namespace mcc
{
	MMNode::~MMNode()
	{
		for (auto& attr : m_attributes)
			delete attr;

		for (auto& pin : m_dataPins)
			delete pin;

		for (auto& pin : m_functionalPins)
			delete pin;
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

	mcd::BlendTreeNode* MMNode::createDatabaseNode(mcd::BlendTree* parent, mcd::MorphemeDB* morphemeDB)
	{
		mcd::BlendTreeNode* node = new mcd::BlendTreeNode(parent, "", 0.f, 0.f, 100.f, 70.f, this->getName(), this->getVersion());
		node->setName(getNodeDefaultName(node, parent));

		parent->addBlendTreeNode(node);

		setupNewBlendTreeNode(node, parent);

		return node;
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

	void MMNode::sortPins()
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

		m_functionalPins.clear();
		m_dataPins.clear();

		for (const auto& pin : sortedPins)
		{
			if (MMFunctionalPin* funcPin = dynamic_cast<MMFunctionalPin*>(pin))
				m_functionalPins.push_back(funcPin);
			else if (MMDataPin* dataPin = dynamic_cast<MMDataPin*>(pin))
				m_dataPins.push_back(dataPin);
		}
	}

	void MMNode::setupNewBlendTreeNode(mcd::BlendTreeNode* node, mcd::BlendTree* parent)
	{
		sortPins();

		// We need to set the default name, but to do this we need a way to count all nodes of the same type in the parent blend tree.

		for (size_t i = 0; i < this->m_attributes.size(); i++)
		{
			mcc::AttributeInfo attrInfo(this->m_attributes[i]);

			attrInfo.createDatabaseAttribute(node->getAttributes());
		}

		for (size_t i = 0; i < this->m_dataPins.size(); i++)
		{
			mcc::MMDataPin* pin = this->m_dataPins[i];

			if (!pin->addToGraphNode(node))
				throw std::runtime_error("Failed to add data pin to graph node: " + pin->getPinName());
		}

		for (size_t i = 0; i < this->m_functionalPins.size(); i++)
		{
			mcc::MMFunctionalPin* pin = this->m_functionalPins[i];

			if (!pin->addToGraphNode(node))
				throw std::runtime_error("Failed to add functional pin to graph node: " + pin->getPinName());
		}
	}

	std::string MMNode::getNodeDefaultName(mcd::BlendTreeNode* node, mcd::BlendTree* parent) const
	{
		char nodeName[256];
		snprintf(nodeName, sizeof(nodeName), "%s%d", this->getName().c_str(), parent->getNumNodesOfType(this->getName()) + 1);

		return std::string(nodeName);
	}
}
