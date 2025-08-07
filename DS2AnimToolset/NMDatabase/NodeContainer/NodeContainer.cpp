#include "NodeContainer.h"

namespace db
{
	Node* NodeContainer::find(std::string name) const
	{
		for (size_t i = 0; i < getNumNodes(); i++)
		{
			Node* node = getNode(i);

			if (node->getName() == name)
				return node;
		}

		return nullptr;
	}

	bool NodeContainer::readValueXML(int format, db::XMLElement* element, LoaderXML* loader)
	{
		for (size_t i = 0; i < element->getNumChildren(); i++)
		{
			XMLElement* child = element->getChild(i);

			if (!child->hasAttribute("name"))
				throw std::runtime_error("NodeContainer::readValueXML: Missing 'name' attribute in XML element.");

			const std::string& name = child->getAttribute("name");

			Node* node = find(name);

			if (!node)
			{
				node = new Node(this, child->getName(), name);
				add(node);
			}

			if (!node->readValueXML(format, child, loader))
				return false;
		}

		return true;
	}

	bool NodeContainer::writeValueXML(int format, SaverXML* saver)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			if (!m_attributes[i]->writeXML(format, saver))
				return false;
		}

		return true;
	}
}