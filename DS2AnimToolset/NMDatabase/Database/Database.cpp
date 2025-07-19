#include "Database.h"

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
		Network* newNetwork = new Network();
		m_networks.addNode(newNetwork);
		return newNetwork;
	}

	void Database::removeNetwork(int index)
	{
		if (index < 0 || index >= m_networks.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_networks.removeNode(index);
	}
}