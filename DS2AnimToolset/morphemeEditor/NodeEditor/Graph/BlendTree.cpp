#include "BlendTree.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	BlendTree::BlendTree(NodeEditor* editor, Graph* parent, const std::string& name) : Graph(editor, parent, name), m_resultNode(nullptr)
	{
		m_resultNode = new Node(editor, this, -1, "Output", nullptr);
		m_resultNode->createInputPin("Output");

		m_resultNode->setPosition(600.0f, 200.0f);
	}

	BlendTree::~BlendTree()
	{
		delete m_resultNode;
	}

	void BlendTree::draw()
	{
		Graph::draw();

		drawResultNode();

		for (Link* link : m_links)
			link->draw();
	}

	bool BlendTree::connectToOutput(Pin* outputPin)
	{
		if (m_resultNode)
		{
			outputPin->connectTo(m_resultNode->getInputPin(0));
			return true;
		}

		return false;
	}

	void BlendTree::drawResultNode()
	{
		if (m_resultNode)
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

			m_resultNode->draw();

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
}
