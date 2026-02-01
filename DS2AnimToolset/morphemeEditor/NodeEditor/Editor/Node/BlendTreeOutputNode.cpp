#include "BlendTreeOutputNode.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	BlendTreeOutputNode::BlendTreeOutputNode(NodeEditor* editor, Graph* parent)
		: Node(editor, parent, -1, "", "Output", nullptr)
	{
		createInputPin("Output");
	}

	void BlendTreeOutputNode::draw()
	{
		StyleSettings& style = m_ownerEditor->getStyleSettings();

		ImNodes::PushColorStyle(ImNodesCol_NodeBackground, style.Colors[NodeEditorStyleCol_ControlParamNodeBackground]);
		ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundHovered, style.Colors[NodeEditorStyleCol_ControlParamNodeBackgroundHovered]);
		ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, style.Colors[NodeEditorStyleCol_ControlParamNodeBackgroundSelected]);

		ImNodes::PushColorStyle(ImNodesCol_NodeOutline, style.Colors[NodeEditorStyleCol_ControlParamNodeOutline]);
		ImNodes::PushColorStyle(ImNodesCol_NodeOutlineHovered, style.Colors[NodeEditorStyleCol_ControlParamNodeOutlineHovered]);
		ImNodes::PushColorStyle(ImNodesCol_NodeOutlineSelected, style.Colors[NodeEditorStyleCol_ControlParamNodeOutlineSelected]);

		ImNodes::PushColorStyle(ImNodesCol_TitleBar, style.Colors[NodeEditorStyleCol_ControlParamNodeTitleBar]);
		ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, style.Colors[NodeEditorStyleCol_ControlParamNodeTitleBarHovered]);
		ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, style.Colors[NodeEditorStyleCol_ControlParamNodeTitleBarSelected]);

		Node::draw();

		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();

		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();

		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
	}
} // namespace NodeEditor
