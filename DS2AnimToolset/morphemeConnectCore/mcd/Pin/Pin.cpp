#include "Pin.h"
#include "mcd/MorphemeDB/MorphemeDB.h"

namespace mcd
{
	bool Pin::CycleDetector::wouldCreateCycle(mcd::Pin* from, mcd::Pin* to)
	{
		return false;
	}

	mcd::GraphNode* Pin::CycleDetector::findTreeRoot(mcd::GraphNode* node)
	{
		return nullptr;
	}

	bool Pin::CycleDetector::depthFirstSearchCycleDetect(mcd::GraphNode* currentNode, mcd::GraphNode* targetNode, std::set<mcd::GraphNode*>& visitedNodes)
	{
		return false;
	}

	bool Pin::CycleDetector::depthFirstSearchRemoveFromList(mcd::GraphNode* currentNode, mcd::GraphNode* targetNode, std::vector<mcd::GraphNode*>& visitedNodes)
	{
		return false;
	}

	bool Pin::breakConnectionTo(Pin* to)
	{
		throw std::runtime_error("Pin::breakConnectionTo() not implemented");
	}

	bool Pin::canConnectTo(Pin* to)
	{
		if (isCompatibleConnectionTarget(to))
		{
			if (canStartConnection() && to->canReceiveConnection(this))
			{
				return CycleDetector::wouldCreateCycle(this, to);
			}
		}
	}

	bool Pin::canReceiveConnection(Pin* from)
	{
		MorphemeDB* db = dynamic_cast<MorphemeDB*>(getDatabase());

		if (db)
			return db->getConnectedFlowEdgeCount(this) == 0;

		// There should be an invalid incident edge check too here, but I am not sure what it means
		return false;
	}

	bool Pin::canStartConnection()
	{
		MorphemeDB* db = dynamic_cast<MorphemeDB*>(getDatabase());

		if (db)
			return db->getConnectedFlowEdgeCount(this) == 0;

		return false;
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

	mcd::Graph* Pin::getParentOrGrandParentGraph()
	{
		if (hasParentNode<mcd::Graph>())
		{
			Graph* graph = dynamic_cast<mcd::Graph*>(getParentNode());

			if (graph)
				return graph;

			if (hasGrandParentNode<mcd::Graph>())
			{
				Graph* grandParentGraph = dynamic_cast<mcd::Graph*>(getGrandParentNode());

				if (grandParentGraph)
					return grandParentGraph;
			}
		}

		return nullptr;
	}
}
