#include "PassDownPinsNode.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	PassDownPinsNode::PassDownPinsNode(Editor* editor, Graph* parent, int id, const std::string& name)
		: Node(editor, parent, id, "", name, nullptr)
	{
	}

	void PassDownPinsNode::draw()
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

	void PassDownPinsNode::updatePins()
	{
		if (!m_parentGraph->isOfType<BlendTree>())
			return;

		BlendTree* parentBlendTree = m_parentGraph->asType<BlendTree>();

		const size_t numPassDownFunctionalPins = parentBlendTree->getNumPassDownPins();

		if (numPassDownFunctionalPins != m_outputDataPins.size())
		{
			for (size_t i = 0; i < m_outputDataPins.size(); i++)
				delete m_outputDataPins[i];

			m_outputDataPins.clear();

			for (size_t i = 0; i < numPassDownFunctionalPins; i++)
			{
				const std::string& pinName = parentBlendTree->getPassDownPinAt(i);
				createOutputPin(pinName);
			}
		}
	}
}