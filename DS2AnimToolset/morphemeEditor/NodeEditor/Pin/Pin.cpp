#include "Pin.h"
#include "NodeEditor/Graph/Graph.h"
#include "NodeEditor/Node/Node.h"
#include "NodeEditor/Link/Link.h"
#include "NodeEditor/imnodes/imnodes.h"

namespace NodeEditor
{
	Pin::Pin(Node* parent, const std::string& name, bool isInput) : Entity(),
		m_parentNode(parent), m_name(name)
	{
		m_type = isInput ? kPinTypeInput : kPinTypeOutput;
	}

	void Pin::draw()
	{
		switch (m_type)
		{
		case NodeEditor::Pin::kPinTypeInput:	return drawInputPin();
		case NodeEditor::Pin::kPinTypeOutput:	return drawOutputPin();
		default:								throw "Unknown pin type in Pin::draw()";
		}
	}

	bool Pin::connectTo(Pin* other)
	{
		if (m_type == other->m_type)
			return false;

		Pin* inputPin = (m_type == kPinTypeInput) ? this : other;
		Pin* outputPin = (m_type == kPinTypeOutput) ? this : other;

		Link* link = new Link(inputPin, outputPin);
		Graph* parentGraph = m_parentNode->getParentGraph();

		if (parentGraph)
		{
			parentGraph->addLink(link);
			return true;
		}

		delete link;
		return false;
	}

	void Pin::drawInputPin()
	{
		ImNodes::BeginInputAttribute(m_id, ImNodesPinShape_TriangleFilled);
		ImGui::TextUnformatted(m_name.c_str());
		ImNodes::EndInputAttribute();
	}

	void Pin::drawOutputPin()
	{
		ImNodes::BeginOutputAttribute(m_id, ImNodesPinShape_TriangleFilled);

		// Get node width in content space
		ImVec2 labelSize = ImGui::CalcTextSize(m_name.c_str());

		ImVec2 nodeSize;
		m_parentNode->calcNodeSize(nodeSize.x, nodeSize.y);

		// Move label to the right edge of the node
		ImGui::Dummy(ImVec2(nodeSize.x - labelSize.x, 0));
		ImGui::SameLine();
		ImGui::TextUnformatted(m_name.c_str());

		ImNodes::EndOutputAttribute();
	}
}
