#include "MMNode.h"

namespace NodeEditor
{
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
				const auto& attrs = json["attributes"];

				if (attrs.is_object())
				{
					for (auto it = attrs.begin(); it != attrs.end(); ++it)
					{
						m_attributes.push_back(
							new MMAttribute(it.value(), it.key())
						);
					}
				}
				else if (attrs.is_array())
				{
					for (const auto& attr : attrs)
					{
						m_attributes.push_back(
							new MMAttribute(attr)
						);
					}
				}
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

			if (m_pinOrder.size() == 0)
			{
				for (auto& pin : m_functionalPins)
					sortedPins.push_back(pin);

				for (auto& pin : m_dataPins)
					sortedPins.push_back(pin);
			}

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

		Node* MMNode::makeNode(Editor* editor, Graph* parent, int id, const std::string& name)
		{
			Node* node = new Node(editor, parent, id, getName(), name, nullptr);

			// Add pins
			std::vector<MMPin*> sortedPins = getSortedPins();

			if (sortedPins.size())
			{
				for (auto& pin : sortedPins)
				{
					if (pin->isOfType<MMDataPin>())
					{
						MMDataPin* dataPin = pin->asType<MMDataPin>();
						DataPin::DataType dataType = DataPin::stringToDataType(dataPin->getDataType());

						if (dataPin->isInput())
							node->createInputDataPin(dataPin->getPinName(), dataType);
						else
							node->createOutputDataPin(dataPin->getPinName(), dataType);
					}
					else if (pin->isOfType<MMFunctionalPin>())
					{
						MMFunctionalPin* funcPin = pin->asType<MMFunctionalPin>();

						if (funcPin->isInput())
							node->createInputPin(funcPin->getPinName());
						else
							node->createOutputPin(funcPin->getPinName());
					}
				}
			}
			else
			{
				for (uint32_t i = 0; i < getNumDataPins(); i++)
				{
					MMDataPin* dataPin = getDataPin(i);
					DataPin::DataType dataType = DataPin::stringToDataType(dataPin->getDataType());
					if (dataPin->isInput())
						node->createInputDataPin(dataPin->getPinName(), dataType);
					else
						node->createOutputDataPin(dataPin->getPinName(), dataType);
				}

				for (uint32_t i = 0; i < getNumFunctionalPins(); i++)
				{
					MMFunctionalPin* funcPin = getFunctionalPin(i);
					if (funcPin->isInput())
						node->createInputPin(funcPin->getPinName());
					else
						node->createOutputPin(funcPin->getPinName());
				}
			}
			
			// Add attributes
			for (uint32_t i = 0; i < getNumAttributes(); i++)
			{
				MMAttribute* mmAttr = getAttribute(i);

				node->addAttribute(mmAttr->makeAttribute(node));
			}

			return node;
		}
	}
}