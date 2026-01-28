#include "Registry.h"
#include "NodeEditor/Entity/Entity.h"
#include "NodeEditor/Graph/Graph.h"

namespace NodeEditor
{
	Registry* Registry::s_instance = nullptr;

	Registry::~Registry()
	{
		for (Entity* node : m_registeredEntities)
			delete node;

		for (Graph* graph : m_registeredGraphs)
			delete graph;

		m_registeredEntities.clear();
		m_registeredGraphs.clear();
	}

	Registry* Registry::getInstance()
	{
		if (!s_instance)
			s_instance = new Registry();

		return s_instance;
	}

	void Registry::destroyInstance()
	{
		if (s_instance)
		{
			delete s_instance;
			s_instance = nullptr;
		}
	}

	void Registry::unregisterEntity(Entity* node)
	{
		auto it = std::find(m_registeredEntities.begin(), m_registeredEntities.end(), node);

		if (it != m_registeredEntities.end())
			m_registeredEntities.erase(it);
	}

	Entity* Registry::findEntity(int id) const
	{
		for (Entity* entity : m_registeredEntities)
		{
			if (entity->getID() == id)
				return entity;
		}

		return nullptr;
	}

	void Registry::unregisterGraph(Graph* graph)
	{
		auto it = std::find(m_registeredGraphs.begin(), m_registeredGraphs.end(), graph);

		if (it != m_registeredGraphs.end())
			m_registeredGraphs.erase(it);
	}

	Graph* Registry::findGraph(int id) const
	{
		for (Graph* graph : m_registeredGraphs)
		{
			if (graph->getID() == id)
				return graph;
		}

		return nullptr;
	}
}
