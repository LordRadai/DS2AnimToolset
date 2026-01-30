#include "ControlParametersNode.h"

namespace NodeEditor
{
	ControlParametersNode::ControlParametersNode(NodeEditorBase* editor, const std::string& name)
		: Node(editor, nullptr, -1, name, nullptr)
	{
	}

	void ControlParametersNode::draw()
	{
        ImNodes::PushColorStyle(ImNodesCol_NodeBackground, IM_COL32(70, 70, 70, 255));
        ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundHovered, IM_COL32(70, 70, 70, 255));
        ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, IM_COL32(70, 70, 70, 255));

        ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(100, 100, 100, 255));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(100, 100, 100, 255));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(100, 100, 100, 255));

		Node::draw();

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
	}
}
