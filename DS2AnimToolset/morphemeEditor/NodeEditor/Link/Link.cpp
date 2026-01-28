#include "NodeEditor/Link/Link.h"
#include "NodeEditor/Pin/Pin.h"
#include "NodeEditor/imnodes/imnodes.h"

namespace NodeEditor
{
	void Link::draw()
	{
		ImNodes::Link(m_id, m_inputPin->getID(), m_outputPin->getID());
	}
}
