#include "MMTransition.h"

namespace NodeEditor
{
	namespace Manifest
	{
		void MMTransition::fromJson(const nlohmann::json& json)
		{
			m_jsonData = json;

			if (json.contains("interfaces") && json["interfaces"].is_array())
			{
				m_interfaces.clear();
				for (const auto& intr : json["interfaces"])
				{
					if (intr.is_string())
						m_interfaces.push_back(intr.get<std::string>());
				}
			}

			if (json.contains("attributes"))
			{
				m_attributes.clear();

				if (json["attributes"].is_object())
				{
					for (auto it = json["attributes"].begin(); it != json["attributes"].end(); ++it)
					{
						MMAttribute* attribute = new MMAttribute(it.value(), it.key());
						m_attributes.push_back(attribute);
					}
				}
				else if (json["attributes"].is_array())
				{
					for (const auto& attrJson : json["attributes"])
					{
						MMAttribute* attribute = new MMAttribute(attrJson);
						m_attributes.push_back(attribute);
					}
				}
			}
		}

		MMAttribute* MMTransition::getAttribute(uint32_t index)
		{
			if (index < m_attributes.size())
				return m_attributes[index];

			throw std::out_of_range("Index out of range for attributes.");
		}

		MMAttribute* MMTransition::findAttribute(const std::string& name)
		{
			for (auto& attr : m_attributes)
			{
				if (attr->getName() == name)
					return attr;
			}

			return nullptr;
		}

		std::string MMTransition::getInterface(uint32_t index) const
		{
			if (index < m_interfaces.size())
				return m_interfaces[index];

			throw std::out_of_range("Index out of range for interfaces.");
		}

		std::string MMTransition::findInterface(const std::string& name) const
		{
			for (const auto& intr : m_interfaces)
			{
				if (intr == name)
					return intr;
			}

			return "";
		}

		Transition* MMTransition::makeTransition(NodeEditor* editor, int id, Node* sourceNode, Node* destinationNode)
		{
			Transition* transit = new Transition(editor, sourceNode->getParentGraph(), id, this->getName(), sourceNode, destinationNode);

			// Create and add attributes
			for (auto& attrManifest : m_attributes)
			{
				Attribute* attribute = attrManifest->makeAttribute(transit);

				transit->addAttribute(attribute);
			}

			return transit;
		}
	}
}