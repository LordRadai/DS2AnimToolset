#pragma once
#include <imgui/imgui.h>
#include "NodeEditor/Pin/Pin.h"
#include "NodeEditor/Entity/Entity.h"
#include <vector>

namespace NodeEditor
{
	class Graph;

	class Node : public Entity
	{
		int m_nodeID;
		std::string m_name;
		std::vector<Pin*> m_inputPins;
		std::vector<Pin*> m_outputPins;

		Graph* m_parentGraph;
		Graph* m_subGraph;

		ImVec2 m_position;
	public:
		Node(Graph* parent, int id, const std::string& name, Graph* subGraph);
		~Node();

		const std::string& getName() const { return m_name; }

		void draw();

		Pin* addInputPin(const std::string& name);
		Pin* addOutputPin(const std::string& name);

		Pin* getInputPin(size_t index) const;
		Pin* getInputPin(const std::string& name) const;

		Pin* getOutputPin(size_t index) const;
		Pin* getOutputPin(const std::string& name) const;

		Graph* getParentGraph() const { return m_parentGraph; }
		Graph* getSubGraph() const { return m_subGraph; }

		void setPosition(float x, float y);

		bool hasSubGraph() const { return m_subGraph != nullptr; }
	};
}