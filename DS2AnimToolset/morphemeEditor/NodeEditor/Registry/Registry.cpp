#include "Registry.h"
#include "NodeEditor/Node/Node.h"
#include "NodeEditor/Graph/Graph.h"

namespace NodeEditor
{
	Registry* Registry::s_instance = nullptr;

	Registry::~Registry()
	{
		for (Node* node : m_registeredNodes)
			delete node;

		for (Graph* graph : m_registeredGraphs)
			delete graph;

		m_registeredNodes.clear();
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

	int Registry::generateUniqueRuntimeID()
	{
		return m_nextRuntimeID++;
	}

	void Registry::unregisterNode(Node* node)
	{
		auto it = std::find(m_registeredNodes.begin(), m_registeredNodes.end(), node);

		if (it != m_registeredNodes.end())
			m_registeredNodes.erase(it);
	}

	void Registry::unregisterGraph(Graph* graph)
	{
		auto it = std::find(m_registeredGraphs.begin(), m_registeredGraphs.end(), graph);

		if (it != m_registeredGraphs.end())
			m_registeredGraphs.erase(it);
	}
}
