#include "Node.h"
#include "NodeEditor/Graph/Graph.h"
#include "NodeEditor/imnodes/imnodes.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	Node::Node(NodeEditor* editor, Graph* parent, int id, const std::string& name, Graph* subGraph) : Entity(editor, name),
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
		StyleSettings& style = m_ownerEditor->getStyleSettings();

		int totalPins = (int)(m_inputPins.size() + m_outputPins.size());
		float pinAreaHeight = totalPins * 10.f;

		float dummyHeight = style.NodeMinContentHeight - pinAreaHeight;
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

	DataPin* Node::createInputDataPin(const std::string& name, DataPin::DataType dataType)
	{
		DataPin* pin = new DataPin(m_ownerEditor, this, name, true, dataType);
		m_inputPins.push_back(pin);
		return pin;
	}

	DataPin* Node::createOutputDataPin(const std::string& name, DataPin::DataType dataType)
	{
		DataPin* pin = new DataPin(m_ownerEditor, this, name, false, dataType);
		m_outputPins.push_back(pin);
		return pin;
	}

	Pin* Node::createInputPin(const std::string& name)
	{
		Pin* pin = new Pin(m_ownerEditor, this, name, true);
		m_inputPins.push_back(pin);

		return pin;
	}

	Pin* Node::createOutputPin(const std::string& name)
	{
		Pin* pin = new Pin(m_ownerEditor, this, name, false);
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

		StyleSettings& style = m_ownerEditor->getStyleSettings();
		ImVec2 textSize = ImGui::CalcTextSize(m_name.c_str());

		const float titleBarHeight = 2.f * imStyle.NodePadding.y + textSize.y;
		const float nodeTotalMinHeight = titleBarHeight + style.NodeMinContentHeight;
		const int totalPins = (int)(m_inputPins.size() + m_outputPins.size());
		const float nodeHeight = totalPins * 10.f;

		width = std::max(style.NodeMinWidth, textSize.x);
		height = std::max(nodeTotalMinHeight, nodeHeight);
	}
}
