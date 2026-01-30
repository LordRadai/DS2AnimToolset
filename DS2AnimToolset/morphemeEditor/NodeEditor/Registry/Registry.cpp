#include "Registry.h"
#include "NodeEditor/Entity/Entity.h"
#include "NodeEditor/Graph/Graph.h"

namespace NodeEditor
{
	Registry::~Registry()
	{
		m_registeredEntities.clear();
		m_registeredGraphs.clear();
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
