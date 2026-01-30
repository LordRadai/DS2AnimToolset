#include "NodeEditor/Link/Link.h"
#include "NodeEditor/Pin/Pin.h"
#include "NodeEditor/imnodes/imnodes.h"

namespace NodeEditor
{
	Link::Link(NodeEditorBase* editor, Pin* inputPin, Pin* outputPin) : Entity(editor, inputPin->getName() + "_" + outputPin->getName()),
		m_inputPin(inputPin), m_outputPin(outputPin)
	{
	}

	void Link::draw()
	{
		ImNodes::Link(m_id, m_inputPin->getID(), m_outputPin->getID());
	}
}
