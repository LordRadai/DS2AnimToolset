#pragma once
#include <vector>

namespace NodeEditor
{
	class Entity;
	class Graph;
	class Editor;

	class Registry
	{
		Editor* m_ownerEditor;

	public:
		Registry(Editor* editor) : m_nextRuntimeID(0), m_nextGraphID(0), m_ownerEditor(editor) {}
		~Registry();

		int generateUniqueRuntimeID() { return m_nextRuntimeID++; }
		int generateUniqueGraphID() { return m_nextGraphID++; }

		void registerEntity(Entity* node) { m_registeredEntities.push_back(node); }
		void unregisterEntity(Entity* node);
		size_t getNumRegisteredEntities() const { return m_registeredEntities.size(); }
		Entity* findEntity(int id) const;
		Entity* getEntityAtIndex(size_t index) const { return m_registeredEntities[index]; }

		void registerGraph(Graph* graph) { m_registeredGraphs.push_back(graph); }
		void unregisterGraph(Graph* graph);
		size_t getNumRegisteredGraphs() const { return m_registeredGraphs.size(); }
		Graph* findGraph(int id) const;
		Graph* getGraphAtIndex(size_t index) const { return m_registeredGraphs[index]; }
	private:
		int m_nextRuntimeID;
		int m_nextGraphID;
		std::vector<Entity*> m_registeredEntities;
		std::vector<Graph*> m_registeredGraphs;
	};
}
