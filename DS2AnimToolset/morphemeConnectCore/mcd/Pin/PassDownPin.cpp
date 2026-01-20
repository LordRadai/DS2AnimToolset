#include "PassDownPin.h"
#include "mcd/Graph/Graph.h"
#include "mcu/Log.h"
#include "mcd/Network/Network.h"
#include "FunctionalPin.h"
#include "DataPin.h"

namespace mcd
{
	PassDownPin::PassDownPin(db::Node* parent, const std::string& name, bool networkOut)
		: Pin(parent, "PassDownPin", name),
		m_multiplicity(std::make_unique<db::EnumAttribute>(this, "Multiplicity", "OneToOne"))
	{
		if (!networkOut)
		{
			//setIsInput(true);
			setReference(true);
		}
	}

	bool PassDownPin::isCompatibleConnectionTarget(Pin* to)
	{
		if (!to)
			return false;

		if (to->isOfType<PassDownPin>())
		{
			PassDownPin* targetPassDownPin = dynamic_cast<PassDownPin*>(to);

			Graph* myChildGraph = getChildGraph();
			Graph* myParentGraph = getParentGraph();
			Graph* targetChildGraph = targetPassDownPin->getChildGraph();
			Graph* targetParentGraph = targetPassDownPin->getParentGraph();

			if (targetChildGraph != myChildGraph)
			{
				if ((targetParentGraph == myChildGraph || targetChildGraph == myParentGraph) && myParentGraph != targetParentGraph)
				{
					if (isInput() != targetPassDownPin->isInput())
						return false;
				}
				else if (targetParentGraph != myParentGraph)
				{
					return false;
				}
			}

			if (isInput() == targetPassDownPin->isInput())
				return false;

			Pin* upstream = recurseUpstreamToNonPassDownPin();

			if (upstream)
			{
				DataPin* firstData = targetPassDownPin->recurseUpstreamToDataPin();
				FunctionalPin* firstFunc = targetPassDownPin->recurseUpstreamToFunctionalPin();

				if ((firstFunc && upstream->isOfType<FunctionalPin>()) ||
					(firstData && upstream->isOfType<DataPin>()))
				{
					return false;
				}
			}

			return true;
		}

		if (to->isOfType<DataPin>())
		{
			DataPin* targetDataPin = dynamic_cast<DataPin*>(to);

			BlendTree* targetBt = targetDataPin->getParentOrGrandParentBlendTree();

			BlendTree* graphCheck = isInput() ? dynamic_cast<BlendTree*>(getChildGraph()) : dynamic_cast<BlendTree*>(getParentGraph());

			if (targetBt != graphCheck)
				return false;

			DataPin* upstream = recurseUpstreamToDataPin();

			if (upstream)
				return upstream->getType() == targetDataPin->getType();
		}

		if (to->isOfType<FunctionalPin>())
		{
			FunctionalPin* targetFunctionalPin = dynamic_cast<FunctionalPin*>(to);

			BlendTree* targetBt = targetFunctionalPin->getParentOrGrandParentBlendTree();
			BlendTree* graphCheck = dynamic_cast<BlendTree*>(getChildGraph());

			if (graphCheck != targetBt)
				graphCheck = dynamic_cast<BlendTree*>(getParentGraph());

			if (graphCheck != targetBt)
				return false;

			if (targetFunctionalPin->isInput())
			{
				FunctionalPin* upstream = recurseUpstreamToFunctionalPin();

				if (upstream)
				{
					if (upstream->isInput())
						return false;

					return upstream->containsFunctionalInterfacesFor(targetFunctionalPin);
				}
			}
			else
			{
				FunctionalPin* downstream = getFirstDownstreamFunctionalPin();

				if (downstream)
				{
					if (!downstream->isInput())
						return false;

					return targetFunctionalPin->containsFunctionalInterfacesFor(downstream);
				}
			}

			FunctionalPin* upstream = recurseUpstreamToFunctionalPin();
			FunctionalPin* downstream = getFirstDownstreamFunctionalPin();

			if (upstream)
				return false;

			if (!downstream)
				return true;

			return getMultiplicity() == Multiplicity::kOneToMany;
		}

		return false;
	}

	bool PassDownPin::canReceiveConnection(Pin* from)
	{
		std::vector<FlowEdge*> edges;

		if (isInput())
		{
			if (hasParentNode<mcd::GraphNode>())
			{
				GraphNode* parentGraphNode = dynamic_cast<GraphNode*>(getParentNode());

				if (parentGraphNode->hasParentNode<mcd::Graph>())
				{
					Graph* parentGraph = dynamic_cast<mcd::Graph*>(parentGraphNode->getParentNode());

					getEdgesConnectedInGraph(edges, parentGraph);
				}
			}
		}

		return edges.empty();
	}

	bool PassDownPin::canConnectTo(Pin* to)
	{
		if (!isCompatibleConnectionTarget(to))
			return false;

		if (!canStartConnection())
			return false;

		if (!to->canReceiveConnection(this))
			return false;

		if (isInput())
		{
			Graph* otherGraph = to->getParentOrGrandParentGraph();
			Graph* myChildGraph = getChildGraph();

			if (otherGraph != myChildGraph)
				return false;
		}

		return !Pin::CycleDetector::wouldCreateCycle(this, to);
	}

	bool PassDownPin::breakConnectionTo(Pin* to)
	{
		if (!to)
		{
			mcu::logError("Invalid parameters passed to mcd::PassDownPin::breakConnectionTo\n");
			return false;
		}

		if (!isDirectlyConnectedTo(to))
		{
			mcu::logError("Trying to break connection between pins that are not directly connected in mcd::PassDownPin::breakConnectionTo\n");
			return false;
		}

		Graph* childGraph = getChildGraph();
		Graph* parentGraph = getParentGraph();
		Graph* graphToUse = nullptr;
		PassDownPin* pinToRemove = this;

		if (to->isOfType<PassDownPin>())
		{
			PassDownPin* targetPassDownPin = dynamic_cast<PassDownPin*>(to);

			Graph* targetChildGraph = targetPassDownPin->getChildGraph();
			Graph* targetParentGraph = targetPassDownPin->getParentGraph();

			if ((childGraph != targetChildGraph && childGraph != targetParentGraph) &&
				(parentGraph != targetChildGraph && parentGraph != targetParentGraph))
			{
				graphToUse = to->getGrandParentGraph();
				pinToRemove = this;
			}
			else
			{
				graphToUse = (childGraph == targetChildGraph || childGraph == targetParentGraph) ? childGraph : parentGraph;
			}
		}
		else
		{
			Graph* targetGraph = to->getParentOrGrandParentGraph();

			if (childGraph != targetGraph && parentGraph != targetGraph)
			{
				graphToUse = to->getGrandParentGraph();
				pinToRemove = this;
			}
			else
			{
				graphToUse = (childGraph == targetGraph) ? childGraph : parentGraph;
			}
		}

		if (graphToUse)
		{
			graphToUse->removeConnection(pinToRemove, to);
			return true;
		}

		mcu::logError("Failed to find graph to remove connection in mcd::PassDownPin::breakConnectionTo\n");
		return false;
	}

	bool PassDownPin::canStartConnection()
	{
		if (getMultiplicity() == Multiplicity::kOneToMany)
			return true;

		std::vector<FlowEdge*> edges;
		mcd::Graph* graph = nullptr;

		if (isInput())
			graph = getChildGraph();
		else
			graph = getParentGraph();

		if (graph)
			getEdgesConnectedInGraph(edges, graph);

		return edges.empty();
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

	mcd::Graph* PassDownPin::getParentGraph()
	{
		if (hasParentNode<mcd::GraphNode>())
		{
			GraphNode* parentGraphNode = dynamic_cast<mcd::GraphNode*>(getParentNode());

			if (parentGraphNode->hasParentNode<mcd::Graph>())
				return dynamic_cast<mcd::Graph*>(parentGraphNode->getParentNode());
		}

		return nullptr;
	}

	PassDownPin::Multiplicity PassDownPin::getMultiplicity() const
	{
		std::string value = m_multiplicity->getValue();

		if (value == "OneToOne")
			return Multiplicity::kOneToOne;
		else if (value == "OneToMany")
			return Multiplicity::kOneToMany;

		throw std::invalid_argument("Invalid multiplicity value in PassDownPin");
	}

	mcd::Pin* PassDownPin::getUpstreamPin()
	{
		if (isReference())
		{
			std::vector<FlowEdge*> edges;
			getConnectedFlowEdges(edges);

			for (size_t i = 0; i < edges.size(); i++)
			{
				FlowEdge* edge = edges[i];

				PassDownPin* downstream = dynamic_cast<PassDownPin*>(edge->getDestinationPin());

				if (downstream == this)
				{
					Pin* upstream = edge->getSourcePin();

					edges.clear();
					return upstream;
				}
			}

			edges.clear();
		}

		if (hasParentNode<GraphNode>())
		{
			GraphNode* parentNode = dynamic_cast<GraphNode*>(getParentNode());

			if (parentNode->hasParentNode<Graph>())
			{
				Graph* parentGraph = dynamic_cast<Graph*>(parentNode->getParentNode());

				for (size_t i = 0; i < parentGraph->getFlowEdgeCount(); i++)
				{
					FlowEdge* edge = parentGraph->getFlowEdge(i);
					PassDownPin* downstream = dynamic_cast<PassDownPin*>(edge->getDestinationPin());

					if (downstream == this)
						return edge->getSourcePin();
				}
			}
		}

		Graph* graph = nullptr;

		if (hasParentNode<GraphNode>())
			graph = dynamic_cast<GraphNode*>(getParentNode())->getGraph();
		else if (hasParentNode<Network>())
			graph = dynamic_cast<Network*>(getParentNode())->getGraph();

		if (graph)
		{
			for (size_t i = 0; i < graph->getFlowEdgeCount(); i++)
			{
				FlowEdge* edge = graph->getFlowEdge(i);
				PassDownPin* downstream = dynamic_cast<PassDownPin*>(edge->getDestinationPin());

				if (downstream == this)
					return edge->getSourcePin();
			}
		}

		return nullptr;
	}

	mcd::Pin* PassDownPin::recurseUpstreamToNonPassDownPin()
	{
		Pin* currentPin = this;

		while (currentPin)
		{
			currentPin = getUpstreamPin();

			if (currentPin && !currentPin->isOfType<PassDownPin>())
				return currentPin;
		}

		return nullptr;
	}

	mcd::FunctionalPin* PassDownPin::recurseUpstreamToFunctionalPin()
	{
		Pin* upstreamPin = recurseUpstreamToNonPassDownPin();

		if (upstreamPin && upstreamPin->isOfType<FunctionalPin>())
			return dynamic_cast<FunctionalPin*>(upstreamPin);
	}

	mcd::DataPin* PassDownPin::recurseUpstreamToDataPin()
	{
		Pin* upstreamPin = recurseUpstreamToNonPassDownPin();

		if (upstreamPin && upstreamPin->isOfType<DataPin>())
			return dynamic_cast<DataPin*>(upstreamPin);
	}

	void PassDownPin::getDownstreamPins(std::vector<mcd::Pin*>& outPins)
	{
		if (isReference())
		{
			std::vector<FlowEdge*> edges;
			getConnectedFlowEdges(edges);

			for (size_t i = 0; i < edges.size(); i++)
			{
				FlowEdge* edge = edges[i];
				PassDownPin* src = dynamic_cast<PassDownPin*>(edge->getSourcePin());

				if (src == this)
					outPins.push_back(edge->getDestinationPin());
			}

			edges.clear();

			return;
		}

		Graph* graph = isInput() ? getChildGraph() : getParentGraph();

		if (!graph)
			return;

		for (size_t i = 0; i < graph->getFlowEdgeCount(); i++)
		{
			FlowEdge* edge = graph->getFlowEdge(i);
			PassDownPin* src = dynamic_cast<PassDownPin*>(edge->getSourcePin());

			if (src == this)
				outPins.push_back(edge->getDestinationPin());
		}
	}

	mcd::FunctionalPin* PassDownPin::getFirstDownstreamFunctionalPin()
	{
		std::vector<mcd::Pin*> downstreamPins;
		getDownstreamPins(downstreamPins);

		for (size_t i = 0; i < downstreamPins.size(); i++)
		{
			mcd::Pin* pin = downstreamPins[i];

			if (pin->isOfType<PassDownPin>())
			{
				PassDownPin* passDownPin = dynamic_cast<PassDownPin*>(pin);

				return passDownPin->getFirstDownstreamFunctionalPin();
			}
			else if (pin->isOfType<FunctionalPin>())
			{
				return dynamic_cast<FunctionalPin*>(pin);
			}
		}

		return nullptr;
	}

	mcd::DataPin* PassDownPin::getFirstDownstreamDataPin()
	{
		std::vector<mcd::Pin*> downstreamPins;
		getDownstreamPins(downstreamPins);

		for (size_t i = 0; i < downstreamPins.size(); i++)
		{
			mcd::Pin* pin = downstreamPins[i];

			if (pin->isOfType<PassDownPin>())
			{
				PassDownPin* passDownPin = dynamic_cast<PassDownPin*>(pin);

				return passDownPin->getFirstDownstreamDataPin();
			}
			else if (pin->isOfType<DataPin>())
			{
				return dynamic_cast<DataPin*>(pin);
			}
		}

		return nullptr;
	}

	mcd::DataPin* PassDownPin::recurseDownstreamToFirstDataPin()
	{
		std::vector<mcd::Pin*> downstreamPins;
		getDownstreamPins(downstreamPins);

		for (size_t i = 0; i < downstreamPins.size(); i++)
		{
			mcd::Pin* pin = downstreamPins[i];

			if (!pin)
				continue;

			if (pin->isOfType<PassDownPin>())
				return dynamic_cast<PassDownPin*>(pin)->recurseDownstreamToFirstDataPin();
			else if (pin->isOfType<DataPin>())
				return dynamic_cast<DataPin*>(pin);
		}

		return nullptr;
	}
}