#include "MorphemeDB.h"

namespace mcd
{
	MorphemeDB::MorphemeDB() : db::Database("MorphemeDB", "MorphemeDB"), m_networks(this, "Networks") {};

	void MorphemeDB::createNetwork(const std::string& name)
	{
		if (m_networks.size() > 0)
			return; // Only one network is allowed.
	}

	Network* MorphemeDB::getNetwork() const
	{
		if (m_networks.size() == 0)
			return nullptr;

		return dynamic_cast<Network*>(m_networks[0]);
	}
}
