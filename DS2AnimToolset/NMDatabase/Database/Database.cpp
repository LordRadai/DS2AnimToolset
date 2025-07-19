#include "Database.h"

namespace db
{
	Network* Database::getNetwork(int index) const
	{
		if (index < 0 || index >= m_networks.size())
			throw std::out_of_range("Index out of range");
		return m_networks[index];
	}

	Network* Database::addNetwork()
	{
		m_networks.push_back(new Network());
		return m_networks.back();
	}

	void Database::removeNetwork(int index)
	{
		if (index < 0 || index >= m_networks.size())
			throw std::out_of_range("Index out of range");
		delete m_networks[index];
		m_networks.erase(m_networks.begin() + index);
	}
}