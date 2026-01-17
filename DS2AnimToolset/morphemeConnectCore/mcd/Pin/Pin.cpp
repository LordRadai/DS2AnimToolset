#include "Pin.h"
#include "mcd/MorphemeDB/MorphemeDB.h"
#include "PassDownPin.h"
#include "mcd/ControlParameter/ControlParameter.h"
#include "mcd/Graph/Graph.h"
#include "mcd/Edge/FlowEdge.h"

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

	void Pin::setIsInput(bool isInput)
	{
		removeAttribute(m_isInput.get());

		if (isInput)
			addAttribute(m_isInput.get());

		m_isInput->setValue(isInput);
	}

	void Pin::setIsArray(bool isArray)
	{
		removeAttribute(m_isArray.get());

		if (isArray)
			addAttribute(m_isArray.get());

		m_isArray->setValue(isArray);
	}

	void Pin::setReferenceTarget(const std::string& referenceTarget)
	{
		removeAttribute(m_referenceTarget.get());

		if (!referenceTarget.empty())
			addAttribute(m_referenceTarget.get());

		m_referenceTarget->setValue(referenceTarget);
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
		MorphemeDB* db = dynamic_cast<MorphemeDB*>(getDatabase());

		if (!canConnectTo(to))
			return nullptr;

		std::string prefix = "";

		Node* sourceNode = getParentNode();

		if (sourceNode->isOfType<mcd::ControlParameter>())
			prefix = "ControlParemeters";

		prefix += sourceNode->getName();

		Graph* ownerGraph = getOwnerGraphForConnection(to);

		if (!ownerGraph)
			throw std::runtime_error("Unable to find owner graph when trying to create edge");

		FlowEdge* newEdge = new FlowEdge(ownerGraph, "FlowEdge", to, this);

		Node* targetNode = to->getParentNode();

		char edgeName[256];
		snprintf(edgeName, 256, "%s_%s_To_%s_%s", prefix.c_str(), sourceNode->getName().c_str(), targetNode->getName().c_str(), to->getName().c_str());

		mcd::FlowEdge* edge = new mcd::FlowEdge(ownerGraph, edgeName, to, this);
		ownerGraph->m_flowEdges->add(edge);

		// TODO: Handle emitted control parameters

		return edge;
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

	mcd::Graph* Pin::getOwnerGraphForConnection(Pin* to)
	{
		Node* otherParentNode = to->getParentNode();
		Graph* graph = nullptr;

		if (otherParentNode)
		{
			if (to->hasParentNode<mcd::Graph>())
				graph = dynamic_cast<mcd::Graph*>(otherParentNode);
			else if (to->hasGrandParentNode<mcd::Graph>())
				graph = dynamic_cast<mcd::Graph*>(to->getGrandParentNode());
		}

		if (hasParentNode<mcd::ControlParameter>())
			return graph;

		if (!hasParentNode<mcd::BlendTreeNode>() && !hasParentNode<mcd::StateMachineNode>())
			return nullptr;

		if (isOfType<mcd::PassDownPin>() && getIsInput())
			return graph;

		return getParentOrGrandParentGraph();
	}
}
