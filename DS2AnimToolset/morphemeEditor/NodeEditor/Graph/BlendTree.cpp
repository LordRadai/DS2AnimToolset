#include "BlendTree.h"
#include "NodeEditor/Pin/DataPin.h"

namespace NodeEditor
{
	BlendTree::BlendTree(NodeEditorBase* editor, Graph* parent, const std::string& name) : Graph(editor, parent, name)
	{
		m_controlParametersNode = new Node(editor, this, -1, "ControlParameters", nullptr);
	}

	BlendTree::~BlendTree()
	{
		delete m_controlParametersNode;
	}

	void BlendTree::draw()
	{
		Graph::draw();

		ImNodes::PushColorStyle(ImNodesCol_NodeBackground, IM_COL32(70, 70, 70, 255));
		ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundHovered, IM_COL32(70, 70, 70, 255));
		ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, IM_COL32(70, 70, 70, 255));

		ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(100, 100, 100, 255));
		ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(100, 100, 100, 255));
		ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(100, 100, 100, 255));

		m_controlParametersNode->draw();

		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();

		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();

		for (Link* link : m_links)
			link->draw();
	}

	void BlendTree::createControlParameterFloat(const std::string& name)
	{
		DataPin* outputPin = new DataPin(m_ownerEditor, m_controlParametersNode, name, false, DataPin::kDataTypeFloat);
		m_controlParametersNode->addOutputPin(outputPin);
	}

	void BlendTree::createControlParameterInt(const std::string& name)
	{
		DataPin* outputPin = new DataPin(m_ownerEditor, m_controlParametersNode, name, false, DataPin::kDataTypeInt);
		m_controlParametersNode->addOutputPin(outputPin);
	}

	void BlendTree::createControlParameterUInt(const std::string& name)
	{
		DataPin* outputPin = new DataPin(m_ownerEditor, m_controlParametersNode, name, false, DataPin::kDataTypeUInt);
		m_controlParametersNode->addOutputPin(outputPin);
	}

	void BlendTree::createControlParameterBool(const std::string& name)
	{
		DataPin* outputPin = new DataPin(m_ownerEditor, m_controlParametersNode, name, false, DataPin::kDataTypeBool);
		m_controlParametersNode->addOutputPin(outputPin);
	}

	void BlendTree::createControlParameterVector3(const std::string& name)
	{
		DataPin* outputPin = new DataPin(m_ownerEditor, m_controlParametersNode, name, false, DataPin::kDataTypeVector3);
		m_controlParametersNode->addOutputPin(outputPin);
	}

	void BlendTree::createControlParameterVector4(const std::string& name)
	{
		DataPin* outputPin = new DataPin(m_ownerEditor, m_controlParametersNode, name, false, DataPin::kDataTypeVector4);
		m_controlParametersNode->addOutputPin(outputPin);
	}

	void BlendTree::createControlParameterQuaternion(const std::string& name)
	{
		DataPin* outputPin = new DataPin(m_ownerEditor, m_controlParametersNode, name, false, DataPin::kDataTypeQuaternion);
		m_controlParametersNode->addOutputPin(outputPin);
	}
}
