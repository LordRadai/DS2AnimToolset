#include "ControlParametersNode.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	ControlParametersNode::ControlParametersNode(Editor* editor, const std::string& name)
		: Node(editor, nullptr, -1, "", name, nullptr)
	{
	}

	void ControlParametersNode::draw()
	{
        updateOutputPins();

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

    void ControlParametersNode::updateOutputPins()
    {
        const int numControlParams = m_ownerEditor->getNumControlParameters();
        if (numControlParams != m_outputDataPins.size())
        {
            for (size_t i = 0; i < m_outputDataPins.size(); i++)
                delete m_outputDataPins[i];

			m_outputDataPins.clear();

            for (size_t i = 0; i < numControlParams; i++)
            {
                ControlParameter* parameter = m_ownerEditor->getControlParameterAtIndex(i);
                createOutputDataPin(parameter->getName(), parameter->getDataType());
            }
        }
    }

    void ControlParametersNode::reset()
    {
        for (size_t i = 0; i < m_attributes.size(); i++)
			delete m_attributes[i];

        for (size_t i = 0; i < m_inputPins.size(); i++)
            delete m_inputPins[i];

        for (size_t i = 0; i < m_outputPins.size(); i++)
            delete m_outputPins[i];

        for (size_t i = 0; i < m_inputDataPins.size(); i++)
			delete m_inputDataPins[i];

		for (size_t i = 0; i < m_outputDataPins.size(); i++)
			delete m_outputDataPins[i];

		m_attributes.clear();
        m_inputPins.clear();
		m_outputPins.clear();
		m_inputDataPins.clear();
		m_outputDataPins.clear();
	}
}
