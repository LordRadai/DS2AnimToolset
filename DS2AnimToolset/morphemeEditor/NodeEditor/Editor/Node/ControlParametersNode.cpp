#include "ControlParametersNode.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	ControlParametersNode::ControlParametersNode(NodeEditor* editor, const std::string& name)
		: Node(editor, nullptr, -1, "", name, nullptr)
	{
	}

	void ControlParametersNode::draw()
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
}
