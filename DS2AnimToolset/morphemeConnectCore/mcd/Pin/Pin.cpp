#include "Pin.h"

namespace mcd
{
	bool Pin::breakConnectionTo(Pin* to)
	{
		throw std::runtime_error("Pin::breakConnectionTo() not implemented");
	}

	bool Pin::canConnectTo(Pin* to)
	{
		throw std::runtime_error("Pin::canConnectTo() not implemented");
	}

	bool Pin::canReceiveConnection(Pin* from)
	{
		throw std::runtime_error("Pin::canReceiveConnection() not implemented");
	}

	bool Pin::canStartConnection()
	{
		throw std::runtime_error("Pin::canStartConnection() not implemented");
	}

	bool Pin::canStartConnectionInGraph(mcd::Graph* graph)
	{
		return canStartConnection();
	}

	void Pin::setReference(bool isReference)
	{
		removeAttribute(m_reference.get());

		if (isReference)
			addAttribute(m_reference.get());

		m_reference->setValue(isReference);
	}

	mcd::FlowEdge* Pin::connectTo(Pin* to)
	{
		throw std::runtime_error("Pin::connectTo() not implemented");
	}
}
