#include "Pin.h"
#include "mcd/MorphemeDB/MorphemeDB.h"
#include "PassDownPin.h"
#include "mcd/ControlParameter/ControlParameter.h"
#include "mcd/Graph/Graph.h"
#include "mcd/Edge/FlowEdge.h"
#include "mcu/Log.h"

namespace mcd
{
	bool Pin::CycleDetector::wouldCreateCycle(mcd::Pin* from, mcd::Pin* to)
	{
		if (to == from)
			return true;

		LOG_TODO("Handle other cycle cases.");

		return false;
	}

	mcd::GraphNode* Pin::CycleDetector::findTreeRoot(mcd::GraphNode* node)
	{
		LOG_NOT_IMPLEMENTED();

		return nullptr;
	}

	bool Pin::CycleDetector::depthFirstSearchCycleDetect(mcd::GraphNode* currentNode, mcd::GraphNode* targetNode, std::set<mcd::GraphNode*>& visitedNodes)
	{
		LOG_NOT_IMPLEMENTED();

		return false;
	}

	bool Pin::CycleDetector::depthFirstSearchRemoveFromList(mcd::GraphNode* currentNode, mcd::GraphNode* targetNode, std::vector<mcd::GraphNode*>& visitedNodes)
	{
		LOG_NOT_IMPLEMENTED();

		return false;
	}

	Pin::Pin(db::Node* parent, const std::string& name, const std::string pinName)
		: db::Node(parent, name, pinName),
		m_isInput(std::make_unique<db::BoolAttribute>(this, "Input", false)),
		m_isArray(std::make_unique<db::BoolAttribute>(this, "Array", false)),
		m_referenceTarget(std::make_unique<db::StringAttribute>(this, "ReferenceTarget", "")),
		m_reference(std::make_unique<db::BoolAttribute>(this, "Reference", false)) 
	{
	};

	bool Pin::breakConnectionTo(Pin* to)
	{
		if (to == nullptr)
		{
			mcu::logError("Invalid parameters passed to mcd::Pin::breakConnectionTo\n");
			return false;
		}

		if (isDirectlyConnectedTo(to))
		{
			mcd::Graph* graph = getParentOrGrandParentGraph();

			if (graph)
			{
				graph->removeConnection(this, to);
				return true;
			}

			if (hasParentNode<mcd::ControlParameter>())
			{
				mcd::Graph* otherGraph = to->getParentOrGrandParentGraph();

				if (otherGraph)
				{
					otherGraph->removeConnection(this, to);
					return true;
				}
			}
			else
			{
				mcu::logErrorf("Invalid parameters passed to mcd::Pin::breakConnectionTo");
			}
		}

		return false;
	}

	bool Pin::canConnectTo(Pin* to)
	{
		if (isCompatibleConnectionTarget(to))
		{
			if (canStartConnection() && to->canReceiveConnection(this))
			{
				return !CycleDetector::wouldCreateCycle(this, to);
			}
		}

		return false;
	}

	bool Pin::canReceiveConnection(Pin* from)
	{
		MorphemeDB* db = dynamic_cast<MorphemeDB*>(getDatabase());

		if (db)
			return db->getConnectedFlowEdgeCount(this) == 0;

		// There should be an invalid incident edge check too here, but I am not sure what it means
		LOG_TODO("Implement invalid incident edge check");

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
		if (to == nullptr)
		{
			mcu::logErrorf("Invalid parameters passed to mcd::Pin::connectTo\n");
			return nullptr;
		}

		MorphemeDB* db = dynamic_cast<MorphemeDB*>(getDatabase());

		if (!canConnectTo(to))
			return nullptr;

		std::string prefix = "";

		Node* sourceNode = getParentNode();

		if (sourceNode->isOfType<mcd::ControlParameter>())
			prefix = "ControlParemeters";

		prefix += sourceNode->getName();

		Graph* ownerGraph = getOwnerGraphForConnection(to);

		if (ownerGraph == nullptr)
		{
			mcu::logWarning("Unable to find valid owning graph when trying to create edge\n");
			return nullptr;
		}

		Node* targetNode = to->getParentNode();

		char edgeName[256];
		snprintf(edgeName, 256, "%s_%s_To_%s_%s", prefix.c_str(), getName().c_str(), targetNode->getName().c_str(), to->getName().c_str());

		mcd::FlowEdge* edge = new mcd::FlowEdge(ownerGraph, edgeName, to, this);
		ownerGraph->addFlowEdge(edge);

		// TODO: Handle emitted control parameters
		LOG_TODO("Handle emitted control parameters");

		return edge;
	}

	mcd::BlendTree* Pin::getParentOrGrandParentBlendTree()
	{
		if (hasParentNode<mcd::BlendTree>())
			return dynamic_cast<mcd::BlendTree*>(getParentNode());

		if (hasGrandParentNode<mcd::BlendTree>())
			return dynamic_cast<mcd::BlendTree*>(getGrandParentNode());

		return nullptr;
	}

	mcd::Graph* Pin::getParentOrGrandParentGraph()
	{
		if (hasParentNode<mcd::Graph>())
		{
			Graph* graph = dynamic_cast<mcd::Graph*>(getParentNode());

			if (graph)
				return graph;
		}

		if (hasGrandParentNode<mcd::Graph>())
		{
			Graph* grandParentGraph = dynamic_cast<mcd::Graph*>(getGrandParentNode());

			if (grandParentGraph)
				return grandParentGraph;
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
				graph = dynamic_cast<mcd::Graph*>(to->getParentNode());
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

	bool Pin::isConnectedTo(Pin* other)
	{
		LOG_NOT_IMPLEMENTED();

		return false;
	}

	bool Pin::isDirectlyConnectedTo(Pin* other)
	{
		if (other->isOfType<mcd::PassDownPin>())
		{
			PassDownPin* passDownPin = dynamic_cast<mcd::PassDownPin*>(other);

			Graph* childGraph = passDownPin->getChildGraph();

			if (childGraph && childGraph->hasEdgeBetweenPins(this, other))
				return true;

			if (passDownPin->hasParentNode<mcd::GraphNode>())
			{
				GraphNode* parentGraphNode = dynamic_cast<mcd::GraphNode*>(passDownPin->getParentNode());

				if (parentGraphNode->hasParentNode<mcd::Graph>())
				{
					Graph* parentGraph = dynamic_cast<mcd::Graph*>(parentGraphNode->getParentNode());

					if (parentGraph->hasEdgeBetweenPins(this, other))
						return true;
				}
			}

			return false;
		}

		mcd::BlendTree* blendTree = getParentOrGrandParentBlendTree();

		if (blendTree && blendTree->hasEdgeBetweenPins(this, other))
			return true;

		return false;
	}
}
