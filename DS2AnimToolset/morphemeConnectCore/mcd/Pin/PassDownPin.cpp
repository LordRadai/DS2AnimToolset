#include "PassDownPin.h"
#include "mcd/Graph/Graph.h"
#include "mcu/Log.h"
#include "mcd/Network/Network.h"

namespace mcd
{
	PassDownPin::PassDownPin(db::Node* parent, const std::string& name)
		: Pin(parent, "PassDownPin", name)
	{
		setReference(true);
	}

	bool PassDownPin::isCompatibleConnectionTarget(Pin* to)
	{
		LOG_NOT_IMPLEMENTED();
		return true;
	}

	bool PassDownPin::canReceiveConnection(Pin* from)
	{
		LOG_NOT_IMPLEMENTED();
		return true;
	}

	bool PassDownPin::canConnectTo(Pin* to)
	{
		LOG_NOT_IMPLEMENTED();
		return true;
	}

	bool PassDownPin::breakConnectionTo(Pin* to)
	{
		LOG_NOT_IMPLEMENTED();
		return true;
	}

	bool PassDownPin::canStartConnection()
	{
		LOG_NOT_IMPLEMENTED();
		return true;
	}

	bool PassDownPin::canStartConnectionInGraph(mcd::Graph* graph)
	{
		LOG_NOT_IMPLEMENTED();
		return true;
	}

	mcd::Graph* PassDownPin::getChildGraph()
	{
		if (hasParentNode<mcd::GraphNode>())
		{
			GraphNode* parentGraphNode = dynamic_cast<mcd::GraphNode*>(getParentNode());

			return parentGraphNode->getGraph();
		}

		if (hasParentNode<mcd::Network>())
		{
			Network* parentNetwork = dynamic_cast<mcd::Network*>(getParentNode());

			return parentNetwork->getGraph();
		}

		return nullptr;
	}
}