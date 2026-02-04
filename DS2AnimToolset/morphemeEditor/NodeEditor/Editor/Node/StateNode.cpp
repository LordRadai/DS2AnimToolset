#include "StateNode.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	StateNode::StateNode(Editor* editor, Graph* parent, int id, const std::string& name, Graph* subGraph)
		: Node(editor, parent, id, "ActiveState", name, subGraph)
	{
	}

	void StateNode::draw()
	{
		StyleSettings& style = m_ownerEditor->getStyleSettings();

		ImNodes::PushStyleVar(ImNodesStyleVar_NodeCornerRounding, style.StateNodeCornerRounding);
		ImNodes::PushStyleVar(ImNodesStyleVar_NodeBorderThickness, style.StateNodeOutlineThickness);

		ImNodes::PushColorStyle(ImNodesCol_NodeBackground, style.Colors[NodeEditorStyleCol_StateNodeBackground]);
		ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundHovered, style.Colors[NodeEditorStyleCol_StateNodeBackgroundHovered]);
		ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, style.Colors[NodeEditorStyleCol_StateNodeBackgroundSelected]);

		ImNodes::PushColorStyle(ImNodesCol_NodeOutline, style.Colors[NodeEditorStyleCol_StateNodeOutline]);
		ImNodes::PushColorStyle(ImNodesCol_NodeOutlineHovered, style.Colors[NodeEditorStyleCol_StateNodeOutlineHovered]);
		ImNodes::PushColorStyle(ImNodesCol_NodeOutlineSelected, style.Colors[NodeEditorStyleCol_StateNodeOutlineSelected]);

		ImNodes::PushColorStyle(ImNodesCol_TitleBar, style.Colors[NodeEditorStyleCol_StateNodeTitleBar]);
		ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, style.Colors[NodeEditorStyleCol_StateNodeTitleBarHovered]);
		ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, style.Colors[NodeEditorStyleCol_StateNodeTitleBarSelected]);

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

		ImNodes::PopStyleVar();
		ImNodes::PopStyleVar();
	}
}
