#include "Pin.h"
#include "NodeEditor/Graph/BlendTree.h"
#include "NodeEditor/Node/Node.h"
#include "NodeEditor/Link/Link.h"
#include "NodeEditor/imnodes/imnodes.h"

namespace NodeEditor
{
	Pin::Pin(NodeEditorBase* editor, Node* parent, const std::string& name, bool isInput) : Entity(editor, name),
		m_parentNode(parent), m_isInput(isInput)
	{
	}

	void Pin::draw()
	{	
		drawInternal(ImNodesPinShape_TriangleFilled, ImNodes::GetStyle().Colors[ImNodesCol_Pin]);
	}

	bool Pin::connectTo(Pin* other)
	{
		if (m_isInput == other->m_isInput)
			return false;

		Graph* parentGraph = m_parentNode->getParentGraph();

		if (!parentGraph->isOfType<BlendTree>())
			return false;

		Pin* inputPin = (m_isInput) ? this : other;
		Pin* outputPin = (!m_isInput) ? this : other;

		Link* link = new Link(m_ownerEditor, inputPin, outputPin);

		parentGraph->asType<BlendTree>()->addLink(link);

		return true;
	}

	void Pin::drawInternal(ImNodesPinShape_ shape, ImColor color)
	{
		ImNodes::PushColorStyle(ImNodesCol_Pin, color);
		ImNodes::PushColorStyle(ImNodesCol_PinHovered, color);

		if (m_isInput)
		{
			ImNodes::BeginInputAttribute(m_id, shape);
			ImGui::TextUnformatted(m_name.c_str());
			ImNodes::EndInputAttribute();
		}
		else
		{
			ImNodes::BeginOutputAttribute(m_id, shape);

			// Get node width in content space
			ImVec2 labelSize = ImGui::CalcTextSize(m_name.c_str());

			ImVec2 nodeSize = m_parentNode->getSize();

			// Move label to the right edge of the node
			ImGui::Dummy(ImVec2(nodeSize.x - labelSize.x, 0));
			ImGui::SameLine();
			ImGui::TextUnformatted(m_name.c_str());

			ImNodes::EndOutputAttribute();
		}

		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
	}
}
