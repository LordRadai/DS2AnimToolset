#include "NodeContainer.h"
#include "mcnSerializer/mcnSerializer.h"

namespace db
{
	bool NodeContainer::isValid() const 
	{
		if (!Node::isValid())
			return false;

		for (const auto& node : m_nodes)
		{
			if (!node->isValid())
				return false;
		}

		return true;
	}

	tinyxml2::XMLElement* NodeContainer::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* containerElement = mcnSerializer::createNodeContainerElement(parent, m_identifier);

		for (const auto& node : m_nodes)
			tinyxml2::XMLElement* nodeElement = node->serialize(containerElement);

		return containerElement;
	}

	void NodeContainer::removeNode(int index) {
		if (index < 0 || index >= m_nodes.size())
			throw std::out_of_range("Index out of range");

		m_nodes.erase(m_nodes.begin() + index);
	}

	Node* NodeContainer::getNode(int index) const {
		if (index < 0 || index >= m_nodes.size())
			throw std::out_of_range("Index out of range");

		return m_nodes[index];
	}

}