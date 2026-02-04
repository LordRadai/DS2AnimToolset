#pragma once
#include "morpheme/mrMessageDistributor.h"
#include "NodeEditor/NodeEditor.h"

class MessageProcessor
{
public:
	NodeEditor::Message* processMessage(NodeEditor::Editor* editor, MR::MessageDistributor* message, const char* name);

private:
	const char* messageTypeAsString(MR::MessageType type);
};