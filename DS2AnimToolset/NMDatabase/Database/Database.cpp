#include "Database.h"
#include "mcnSerializer/mcnSerializer.h"

namespace db
{
	Network* Database::getNetwork(int index) const
	{
		if (index < 0 || index >= m_networks.getNumNodes())
			throw std::out_of_range("Index out of range");

		return static_cast<Network*>(m_networks.getNode(index));
	}

	Network* Database::addNetwork()
	{
		Network* newNetwork = new Network(this);
		m_networks.addNode(newNetwork);
		return newNetwork;
	}

	void Database::removeNetwork(int index)
	{
		if (index < 0 || index >= m_networks.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_networks.removeNode(index);
	}

	bool Database::isValid() const
	{
		return Node::isValid() &&
			m_namespace == "mcd" &&
			m_networks.getNumNodes() == 1 &&
			m_networks.isValid();
	}

	tinyxml2::XMLElement* Database::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* dbElement = Node::serialize(parent);

		tinyxml2::XMLElement* networksElement = m_networks.serialize(dbElement);
		networksElement->SetName("Networks");

		if (networksElement)
			dbElement->InsertEndChild(networksElement);

		return dbElement;
	}
}