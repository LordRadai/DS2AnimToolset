#include "Node.h"
#include "NodeEditor/Graph/Graph.h"
#include "NodeEditor/imnodes/imnodes.h"
#include "NodeEditor/StyleSettings.h"

namespace NodeEditor
{
	Node::Node(Graph* parent, int id, const std::string& name, Graph* subGraph) : Entity(name),
		m_parentGraph(parent), m_nodeID(id), m_subGraph(subGraph), m_position(ImVec2(0.f, 0.f))
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

		ImVec2 textSize = ImGui::CalcTextSize(m_name.c_str());

		float nodeWidth, nodeHeight;
		calcNodeSize(nodeWidth, nodeHeight);

		// ---- Title bar ----
		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted(m_name.c_str());
		ImNodes::EndNodeTitleBar();

		// ---- Content area ----
		StyleSettings& style = getStyleSettings();

		float pinAreaHeight = style.nodeMinContentHeight;
		int totalPins = (int)(m_inputPins.size() + m_outputPins.size());

		float dummyHeight = pinAreaHeight - totalPins * style.nodePinSpacing;
		if (dummyHeight < 0.0f) dummyHeight = 0.0f;

		// Add a dummy to enforce minimum node height
		ImGui::Dummy(ImVec2(nodeWidth, dummyHeight));

		// ---- Draw pins ----
		for (Pin* inputPin : m_inputPins)
			inputPin->draw();

		for (Pin* outputPin : m_outputPins)
			outputPin->draw();

		// ---- Node position ----
		ImNodes::SetNodeGridSpacePos(m_id, m_position);

		ImNodes::EndNode();
	}

	ImVec2 Node::getSize() const
	{
		float width, height;
		calcNodeSize(width, height);

		return ImVec2(width, height);
	}

	ImVec2 Node::getCenter() const
	{
		float width, height;
		calcNodeSize(width, height);

		return ImVec2(m_position.x + width * 0.5f, m_position.y + height * 0.5f);
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

	const std::string Node::getFullName() const
	{
		if (m_parentGraph)
			return m_parentGraph->getFullName() + "|" + m_name;

		return m_name;
	}

	void Node::calcNodeSize(float& width, float& height) const
	{
		ImNodesStyle& imStyle = ImNodes::GetStyle();

		StyleSettings& style = getStyleSettings();
		ImVec2 textSize = ImGui::CalcTextSize(m_name.c_str());

		const float titleBarHeight = 2.f * imStyle.NodePadding.y + textSize.y;
		const float nodeTotalMinHeight = titleBarHeight + style.nodeMinContentHeight;
		const int totalPins = (int)(m_inputPins.size() + m_outputPins.size());
		const float nodeHeight = totalPins * style.nodePinSpacing;

		width = std::max(style.nodeMinWidth, textSize.x);
		height = std::max(nodeTotalMinHeight, nodeHeight);
	}
}
