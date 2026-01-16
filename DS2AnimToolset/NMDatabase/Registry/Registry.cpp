#include "Registry.h"

namespace db
{
	Registry* Registry::g_Registry = nullptr;

	Registry::Registry() :
		m_enableRuntimeIDGeneration(true),
		m_nextRuntimeID(0),
		m_nextNodeID(0),
		m_nextTypeID(0)
	{
	}

	Registry::~Registry()
	{
	}

	Registry* Registry::getInstance()
	{
		if (g_Registry == nullptr)
			g_Registry = new Registry();

		return g_Registry;
	}

	void Registry::shutdown()
	{
		if (g_Registry)
		{
			delete g_Registry;
			g_Registry = nullptr;
		}
	}

	int Registry::getNewRuntimeID()
	{
		if (m_enableRuntimeIDGeneration)
			return m_nextRuntimeID++;

		return -1;
	}

	int Registry::getNewNodeID()
	{
		return m_nextNodeID++;
	}

	int Registry::getNewTypeID()
	{
		return m_nextTypeID++;
	}
}
