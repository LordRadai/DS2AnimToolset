#pragma once
#include <imgui/imgui.h>
#include "NodeEditor/Pin/Pin.h"
#include "NodeEditor/Pin/DataPin.h"
#include "NodeEditor/Entity/Entity.h"
#include <vector>

namespace NodeEditor
{
	class Graph;

	class Node : public Entity
	{
	protected:
		Graph* m_parentGraph;
		int m_nodeID;
		std::vector<Pin*> m_inputPins;
		std::vector<Pin*> m_outputPins;
		ImVec2 m_position;
		Graph* m_subGraph;

	public:
		Node(NodeEditorBase* editor, Graph* parent, int id, const std::string& name, Graph* subGraph);
		virtual ~Node() override;

		void draw() override;

		ImVec2 getPosition() const { return m_position; }
		ImVec2 getCenter() const;
		ImVec2 getSize() const;

		DataPin* createInputDataPin(const std::string& name, DataPin::DataType dataType);
		DataPin* createOutputDataPin(const std::string& name, DataPin::DataType dataType);

		Pin* createInputPin(const std::string& name);
		Pin* createOutputPin(const std::string& name);

		void addInputPin(Pin* pin) { m_inputPins.push_back(pin); }
		void addOutputPin(Pin* pin) { m_outputPins.push_back(pin); }

		Pin* getInputPin(size_t index) const;
		Pin* getInputPin(const std::string& name) const;

		Pin* getOutputPin(size_t index) const;
		Pin* getOutputPin(const std::string& name) const;

		Graph* getParentGraph() const { return m_parentGraph; }
		Graph* getSubGraph() const { return m_subGraph; }

		void setPosition(float x, float y);

		bool hasSubGraph() const { return m_subGraph != nullptr; }

		const std::string getFullName() const;

	private:
		void calcNodeSize(float& width, float& height) const;
	};
}