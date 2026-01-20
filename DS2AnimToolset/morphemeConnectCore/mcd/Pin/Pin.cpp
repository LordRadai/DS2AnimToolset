#include "Pin.h"
#include "mcd/MorphemeDB/MorphemeDB.h"
#include "PassDownPin.h"
#include "mcd/ControlParameter/ControlParameter.h"
#include "mcd/Graph/Graph.h"
#include "mcd/Edge/FlowEdge.h"
#include "mcu/Log.h"
#include "DataPin.h"
#include "FunctionalPin.h"
#include "PassDownPin.h"

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
		if (!isCompatibleConnectionTarget(to))
			return false;

		if (!canStartConnection())
			return false;

		if (!to->canReceiveConnection(this))
			return false;

		return !CycleDetector::wouldCreateCycle(this, to);
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

		if (isOfType<mcd::PassDownPin>() && isInput())
			return graph;

		return getParentOrGrandParentGraph();
	}

	bool Pin::isConnectedTo(Pin* other)
	{
		if (other == nullptr)
			return false;

		std::vector<mcd::FlowEdge*> connectedEdges;
		other->getConnectedFlowEdges(connectedEdges);

		for (size_t i = 0; i < connectedEdges.size(); i++)
		{
			mcd::FlowEdge* edge = connectedEdges[i];

			if (edge == nullptr)
				continue;

			std::vector<Pin*> otherEnds;
			edge->getOtherEnd(otherEnds, other);

			for (size_t j = 0; j < otherEnds.size(); j++)
			{
				Pin* endPin = otherEnds[j];

				if (endPin == this)
					return true;
			}
		}

		return false;
	}

	bool Pin::hasIncidentEdge()
	{
		if (!getDatabase()->isOfType<MorphemeDB>())
			return false;

		MorphemeDB* db = dynamic_cast<MorphemeDB*>(getDatabase());

		return db->getConnectedFlowEdgeCount(this) != 0;
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

	void Pin::getEdgesConnectedInGraph(std::vector<mcd::FlowEdge*>& outEdges, mcd::Graph* graph)
	{
		if (graph)
		{
			for (size_t i = 0; i < graph->getFlowEdgeCount(); i++)
			{
				mcd::FlowEdge* currentEdge = graph->getFlowEdge(i);

				if (currentEdge->getSourcePin() == this || currentEdge->getDestinationPin() == this)
					outEdges.push_back(currentEdge);
			}
		}
	}

	void Pin::getConnectedFlowEdges(std::vector<mcd::FlowEdge*>& outEdges)
	{
		if (!getDatabase()->isOfType<MorphemeDB>())
			return;

		MorphemeDB* db = dynamic_cast<MorphemeDB*>(getDatabase());

		int edgeCount = db->getConnectedFlowEdgeCount(this);
		outEdges.reserve(edgeCount);

		for (int i = 0; i < edgeCount; i++)
		{
			mcd::FlowEdge* edge = db->getConnectedFlowEdge(this, i);
			outEdges.push_back(edge);
		}
	}

	mcd::Graph* Pin::getGrandParentGraph()
	{
		if (hasGrandParentNode<mcd::Graph>())
			return dynamic_cast<mcd::Graph*>(getGrandParentNode());

		return nullptr;
	}

	mcd::FunctionalPin* Pin::asFunctionalPin()
	{
		if (isOfType<mcd::FunctionalPin>())
			return dynamic_cast<mcd::FunctionalPin*>(this);

		return nullptr;
	}

	mcd::DataPin* Pin::asDataPin()
	{
		if (isOfType<mcd::DataPin>())
			return dynamic_cast<mcd::DataPin*>(this);

		return nullptr;
	}

	mcd::PassDownPin* Pin::asPassDownPin()
	{
		if (isOfType<mcd::PassDownPin>())
			return dynamic_cast<mcd::PassDownPin*>(this);

		return nullptr;
	}

	bool Pin::containsFunctionalInterfacesFor(Pin* other)
	{
		std::vector<std::string> myInterfaces;
		getAllFunctionalInterfaces(myInterfaces);

		std::vector<std::string> otherInterfaces;
		other->getAllFunctionalInterfaces(otherInterfaces);

		for (size_t i = 0; i < otherInterfaces.size(); i++)
		{
			const std::string& interfaceName = otherInterfaces[i];
			bool found = false;

			for (size_t j = 0; j < myInterfaces.size(); j++)
			{
				if (myInterfaces[j] == interfaceName)
				{
					found = true;
					break;
				}
			}

			if (!found)
				return false;
		}

		return true;
	}

	void Pin::getAllFunctionalInterfaces(std::vector<std::string>& outInterfaces)
	{
		std::vector<std::string> passedThroughInterfaces;

		getPassedThroughFunctionalInterfaces(outInterfaces);
		getPassedThroughFunctionalInterfaces(passedThroughInterfaces);

		outInterfaces.insert(outInterfaces.end(), passedThroughInterfaces.begin(), passedThroughInterfaces.end());
	}

	void Pin::getConnectedPins(std::vector<mcd::Pin*>& outPins)
	{
		std::vector<mcd::FlowEdge*> edges;
		getConnectedFlowEdges(edges);

		for (size_t i = 0; i < edges.size(); i++)
		{
			mcd::FlowEdge* edge = edges[i];

			edge->getOtherEnd(outPins, this);
		}
	}

	bool Pin::hasDownstreamConnection()
	{
		LOG_NOT_IMPLEMENTED();
		return false;
	}
}
