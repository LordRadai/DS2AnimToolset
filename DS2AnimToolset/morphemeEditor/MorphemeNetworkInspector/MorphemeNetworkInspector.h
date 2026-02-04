#pragma once
#include "NodeEditor/NodeEditor.h"
#include "morpheme/mrNetworkDef.h"
#include "NodeProcessor/NodeProcessor.h"
#include "MessageProcessor/MessageProcessor.h"

class MorphemeNetworkInspector : public NodeEditor::Editor
{
	NodeProcessor m_nodeProcessor;
	MessageProcessor m_messageProcessor;
public:
	MorphemeNetworkInspector() : NodeEditor::Editor(NodeEditor::NodeEditorFlags_ReadOnly) {}
	~MorphemeNetworkInspector() override {}

	bool loadNetwork(MR::NetworkDef* network);
};