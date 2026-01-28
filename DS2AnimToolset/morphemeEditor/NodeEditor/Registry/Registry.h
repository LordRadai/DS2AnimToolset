#pragma once
#include <vector>

namespace NodeEditor
{
	class Node;
	class Graph;

	class Registry
	{
		static Registry* s_instance;

	public:
		static Registry* getInstance();
		static void destroyInstance();

		int generateUniqueRuntimeID();

		void registerNode(Node* node) { m_registeredNodes.push_back(node); }
		void unregisterNode(Node* node);
		size_t getNumRegisteredNodes() const { return m_registeredNodes.size(); }

		void registerGraph(Graph* graph) { m_registeredGraphs.push_back(graph); }
		void unregisterGraph(Graph* graph);
		size_t getNumRegisteredGraphs() const { return m_registeredGraphs.size(); }
	private:
		int m_nextRuntimeID;
		std::vector<Node*> m_registeredNodes;
		std::vector<Graph*> m_registeredGraphs;

		Registry() : m_nextRuntimeID(0) {}
		~Registry();
	};
}
