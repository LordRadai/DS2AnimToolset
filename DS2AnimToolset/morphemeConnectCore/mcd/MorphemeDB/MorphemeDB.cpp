#include "MorphemeDB.h"
#include "mcu/Log.h"

namespace mcd
{
	MorphemeDB::MorphemeDB() : db::Database("mcd", "MorphemeDB", "MorphemeDB"), 
		m_networks(std::make_unique<db::TypedNodeContainer<mcd::Network>>(this, "Networks"))
	{
		addAttribute(m_networks.get());
	};

	void MorphemeDB::createNetwork(const std::string& name)
	{
		if (m_networks->size() > 0)
		{
			mcu::logErrorf("Cannot create Network %s. A network is already present in the database\n", name.c_str());
			return;
		}

		m_networks->add(new mcd::Network(this, name));
	}

	void MorphemeDB::removeNetwork()
	{
		if (m_networks->size() == 0)
			return;

		m_networks->remove(m_networks->getNode(0));
	}

	Network* MorphemeDB::getNetwork() const
	{
		if (m_networks->size() == 0)
			return nullptr;

		return m_networks->getNode(0);
	}

	int MorphemeDB::getConnectedFlowEdgeCount(const mcd::Pin* pin)
	{
		LOG_NOT_IMPLEMENTED();

		return 0;
	}

	mcd::FlowEdge* MorphemeDB::getConnectedFlowEdge(const mcd::Pin* pin, int index)
	{
		LOG_NOT_IMPLEMENTED();

		return nullptr;
	}
}
