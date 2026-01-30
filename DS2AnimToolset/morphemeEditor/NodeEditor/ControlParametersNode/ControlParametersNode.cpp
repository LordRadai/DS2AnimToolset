#include "ControlParametersNode.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	ControlParametersNode::ControlParametersNode(NodeEditor* editor, const std::string& name)
		: Node(editor, nullptr, -1, name, nullptr)
	{
	}

	void ControlParametersNode::draw()
	{
		StyleSettings& style = m_ownerEditor->getStyleSettings();

        ImNodes::PushColorStyle(ImNodesCol_NodeBackground, style.ControlParametersNodeBackground);
        ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundHovered, style.ControlParametersNodeBackgroundHovered);
        ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, style.ControlParametersNodeBackgroundSelected);

        ImNodes::PushColorStyle(ImNodesCol_TitleBar, style.ControlParametersNodeTitleBar);
        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, style.ControlParametersNodeTitleBarHovered);
        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, style.ControlParametersNodeTitleBarSelected);

		Node::draw();

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
	}
}
