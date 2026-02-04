#include "MessageProcessor.h"
#include "RLog/RLog.h"
#include "extern.h"

NodeEditor::Message* MessageProcessor::processMessage(NodeEditor::Editor* editor, MR::MessageDistributor* message, const char* name)
{
	if (message->m_messageType != MESSAGE_TYPE_REQUEST)
	{
		g_appLog->debugMessage(MsgLevel_Warn, "MessageProcessor::processMessage: Unsupported message type '%s' for message '%s'.\n",
			messageTypeAsString(message->m_messageType), name);

		return nullptr;
	}

	return editor->createMessage(message->m_messageID, name, messageTypeAsString(message->m_messageType));
}

const char* MessageProcessor::messageTypeAsString(MR::MessageType type)
{
	switch (type)
	{
	case MESSAGE_TYPE_REQUEST:					return "Request";
	case MESSAGE_TYPE_HIT:						return "Hit";
	case MESSAGE_TYPE_RETARGET_UPDATE_OFFSETS:	return "RetargetUpdateOffsets";
	case MESSAGE_TYPE_RETARGET_UPDATE_SCALE:	return "RetargetUpdateScale";
	case MESSAGE_TYPE_SCALE_CHARACTER:			return "ScaleCharacter";
	default:									return "Unknown";
	}
}