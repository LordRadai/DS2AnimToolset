#include "Node.h"
#include "NodeEditor/Graph/Graph.h"
#include "NodeEditor/imnodes/imnodes.h"

namespace NodeEditor
{
	Node::Node(Graph* parent, int id, const std::string& name, Graph* subGraph) : Entity(),
		m_parentGraph(parent), m_nodeID(id), m_name(name), m_subGraph(subGraph), m_position(ImVec2(0.f, 0.f))
	{
	}

	Node::~Node()
	{
		for (Pin* inputPin : m_inputPins)
			delete inputPin;

		for (Pin* outputPin : m_outputPins)
			delete outputPin;
	}

	void Node::draw()
	{
		ImNodes::BeginNode(m_id);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted(m_name.c_str());
		ImNodes::EndNodeTitleBar();

		for (Pin* inputPin : m_inputPins)
			inputPin->draw();

		for (Pin* outputPin : m_outputPins)
			outputPin->draw();

		ImNodes::EndNode();
	}

	Pin* Node::addInputPin(const std::string& name)
	{
		Pin* pin = new Pin(this, name, true);
		m_inputPins.push_back(pin);

		return pin;
	}

	Pin* Node::addOutputPin(const std::string& name)
	{
		Pin* pin = new Pin(this, name, false);
		m_outputPins.push_back(pin);

		return pin;
	}

	Pin* Node::getInputPin(size_t index) const
	{
		if (index < m_inputPins.size())
			return m_inputPins[index];

		return nullptr;
	}

	Pin* Node::getInputPin(const std::string& name) const
	{
		for (Pin* pin : m_inputPins)
		{
			if (pin->getName() == name)
				return pin;
		}

		return nullptr;
	}

	Pin* Node::getOutputPin(size_t index) const
	{
		if (index < m_outputPins.size())
			return m_outputPins[index];

		return nullptr;
	}

	Pin* Node::getOutputPin(const std::string& name) const
	{
		for (Pin* pin : m_outputPins)
		{
			if (pin->getName() == name)
				return pin;
		}

		return nullptr;
	}

	void Node::setPosition(float x, float y)
	{
		m_position = ImVec2(x, y);
	}
}
