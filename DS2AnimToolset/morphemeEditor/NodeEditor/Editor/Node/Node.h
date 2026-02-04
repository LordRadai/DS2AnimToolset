#pragma once
#include <imgui/imgui.h>
#include <vector>

#include "NodeEditor/Editor/Pin/Pin.h"
#include "NodeEditor/Editor/Pin/DataPin.h"
#include "NodeEditor/Editor/Entity/Entity.h"
#include "NodeEditor/Editor/Attribute/Attribute.h"

namespace NodeEditor
{
	class Graph;

	class Node : public Entity
	{
	protected:
		Graph* m_parentGraph;
		int m_nodeID;
		std::string m_typeName;
		std::vector<Attribute*> m_attributes;
		std::vector<Pin*> m_inputPins;
		std::vector<DataPin*> m_inputDataPins;
		std::vector<Pin*> m_outputPins;
		std::vector<DataPin*> m_outputDataPins;
		ImVec2 m_position;
		Graph* m_subGraph;

	public:
		Node(Editor* editor, Graph* parent, int id, const std::string typeName, const std::string& name, Graph* subGraph);

		virtual ~Node() override;
		virtual void draw() override;
		virtual bool editorGUI() override;
		virtual const std::string getFullName() const override;

		int getNodeID() const { return m_nodeID; }
		const std::string getTypeName() const { return m_typeName; }

		ImVec2 getPosition() const { return m_position; }
		ImVec2 getCenter() const;
		ImVec2 getSize() const;

		Attribute* createAttribute(const std::string& name, const std::string& type);
		void addAttribute(Attribute* attribute) { m_attributes.push_back(attribute); }
		Attribute* getAttribute(size_t index) const;
		Attribute* getAttribute(const std::string& name) const;
		size_t getNumAttributes() const { return m_attributes.size(); }

		DataPin* createInputDataPin(const std::string& name, DataPin::DataType dataType);
		DataPin* createOutputDataPin(const std::string& name, DataPin::DataType dataType);

		Pin* createInputPin(const std::string& name);
		Pin* createOutputPin(const std::string& name);

		void addInputPin(Pin* pin) { m_inputPins.push_back(pin); }
		void addOutputPin(Pin* pin) { m_outputPins.push_back(pin); }

		Pin* getInputPin(size_t index) const;
		Pin* getInputPin(const std::string& name) const;
		size_t getNumInputPins() const { return m_inputPins.size(); }

		DataPin* getInputDataPin(size_t index) const;
		DataPin* getInputDataPin(const std::string& name) const;
		size_t getNumInputDataPins() const { return m_inputDataPins.size(); }

		Pin* getOutputPin(size_t index) const;
		Pin* getOutputPin(const std::string& name) const;
		size_t getNumOutputPins() const { return m_outputPins.size(); }

		DataPin* getOutputDataPin(size_t index) const;
		DataPin* getOutputDataPin(const std::string& name) const;
		size_t getNumOutputDataPins() const { return m_outputDataPins.size(); }

		Graph* getParentGraph() const { return m_parentGraph; }

		Graph* getSubGraph() const { return m_subGraph; }
		void setSubGraph(Graph* subGraph) { m_subGraph = subGraph; }

		void setPosition(float x, float y);

		bool hasSubGraph() const { return m_subGraph != nullptr; }

		bool setAttribute(const std::string& name, const std::vector<std::any>& values);
		bool setAttribute(const std::string& name, const std::any& value);

	private:
		void calcNodeSize(float& width, float& height) const;
	};
}