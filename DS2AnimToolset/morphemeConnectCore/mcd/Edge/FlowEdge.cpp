#include "FlowEdge.h"
#include "mcd/StateMachine/StateMachine.h"
#include "mcd/Network/Network.h"

namespace mcd
{
	FlowEdge::FlowEdge(CompositeAttribute* parent, std::string name, mcd::Pin* to, mcd::Pin* from)
		: Edge(parent, "FlowEdge", name), 
		m_from(std::make_unique<db::Pointer<mcd::Pin>>(this, "From", from)), 
		m_to(std::make_unique<db::Pointer<mcd::Pin>>(this, "To", to))
	{
		addAttribute(m_from.get());
		addAttribute(m_to.get());
	}

    void FlowEdge::getOtherEnd(std::vector<mcd::Pin*>& outPins, mcd::Pin* sourcePin)
    {
        Pin* fromPin = getSourcePin();
        Pin* toPin = getDestinationPin();

        Pin* otherPin = (sourcePin != fromPin) ? fromPin : toPin;

        if (!otherPin)
            return;

        if (!otherPin->isOfType<PassDownPin>())
        {
            outPins.push_back(otherPin);
            return;
        }

        if (otherPin->hasParentNode<StateMachine>())
        {
            if (!otherPin->isInput())
            {
                if (otherPin->getName() == "Result")
                {
                    outPins.push_back(otherPin);
                    return;
                }
            }

            std::vector<FlowEdge*> edges;
            otherPin->getConnectedFlowEdges(edges);

            for (FlowEdge* edge : edges)
            {
                if (!edge) continue;

                Pin* nextPin =
                    (fromPin != sourcePin) ? edge->getDestinationPin() : edge->getSourcePin();

                if (nextPin == otherPin)
                    edge->getOtherEnd(outPins, otherPin);
            }

            return;
        }

        if (!otherPin->hasParentNode<mcd::Network>())
        {
            std::vector<FlowEdge*> edges;
            otherPin->getConnectedFlowEdges(edges);

            for (FlowEdge* edge : edges)
            {
                if (!edge) continue;

                Pin* nextPin =
                    (fromPin != sourcePin) ? edge->getDestinationPin() : edge->getSourcePin();

                if (nextPin == otherPin)
                    edge->getOtherEnd(outPins, otherPin);
            }
        }

        if (!otherPin->isOfType<PassDownPin>())
            outPins.push_back(otherPin);
    }
}
