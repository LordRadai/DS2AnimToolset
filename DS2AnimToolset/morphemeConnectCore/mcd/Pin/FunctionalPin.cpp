#include "FunctionalPin.h"
#include "PassDownPin.h"
#include "mcu/Log.h"
#include "mcd/Graph/GraphNode.h"

namespace mcd
{
	FunctionalPin::FunctionalPin(db::Node* parent, const std::string& name)
		: Pin(parent, "FunctionalPin", name),
		  m_interfaces(std::make_unique<db::StringArrayAttribute>(this, "Interfaces")),
		m_passThroughEnabled(std::make_unique<db::BoolAttribute>(this, "PassThroughEnabled", false))
	{
		addAttribute(m_interfaces.get());
		addAttribute(m_passThroughEnabled.get());
	}

    bool FunctionalPin::isCompatibleConnectionTarget(Pin* to)
    {
        if (isInput())
            return false;

        if (!to->isOfType<PassDownPin>())
        {
            if (!to->isOfType<FunctionalPin>())
                return false;

            if (!to->isInput())
                return false;

            if ((!isPassThroughEnabled() || dfsHasUpstreamKnownInterfaces()) && !containsFunctionalInterfacesFor(to))
            {
                if (!isPassThroughEnabled())
                    return false;

                if (!hasParentNode<GraphNode>())
                    return false;

                GraphNode* parentGraphNode = dynamic_cast<GraphNode*>(getParentNode());

                std::vector<FunctionalPin*> inputs;
                parentGraphNode->getInputFunctionalPins(GraphNode::InputPinQueryType::kConnectedAndPassThroughEnabled, &inputs);

                for (FunctionalPin* inputPin : inputs)
                {
                    if (inputPin->dfsHasUpstreamKnownInterfaces() && !inputPin->containsFunctionalInterfacesFor(to))
                        return false;
                }
            }

            return true;
        }

        PassDownPin* passDownPin = dynamic_cast<PassDownPin*>(to);

        Node* targetGrandParent = to->getGrandParentNode();
        Node* thisGrandParent = getGrandParentNode();

        if (thisGrandParent == targetGrandParent)
        {
            if (!passDownPin->isInput())
                return false;
        }
        else
        {
            if (passDownPin->isInput())
                return false;
        }

        if (thisGrandParent)
        {
            Node* thisParent = thisGrandParent->getParentNode();
            Node* otherParent = passDownPin->getParentNode();

            if (thisParent == otherParent)
            {
                DataPin* downstream = passDownPin->getFirstDownstreamDataPin();

                if (downstream == nullptr && containsFunctionalInterfacesFor(passDownPin))
                    return true;

                return false;
            }

            if (thisGrandParent != passDownPin->getGrandParentNode())
                return false;
        }

        return false;
    }


	void FunctionalPin::addInterface(const std::string& interfaceName)
	{
		m_interfaces->add(interfaceName);
	}

	void FunctionalPin::removeInterface(const uint32_t index)
	{
		if (index < 0 || index >= m_interfaces->size())
			throw std::out_of_range("FunctionalPin::removeInterface() - Index out of range");

		m_interfaces->removeAt(index);
	}

	void FunctionalPin::setPassThroughEnabled(bool enabled)
	{
		m_passThroughEnabled->setValue(enabled);
	}

	std::string FunctionalPin::findInterface(const std::string& name) const
	{
		for (size_t i = 0; i < m_interfaces->size(); i++)
		{
			std::string element = m_interfaces->getElement(i);

			if (element == name)
				return element;
		}

		return "";
	}

    bool FunctionalPin::dfsHasUpstreamKnownInterfaces()
    {
        if (!isPassThroughEnabled())
            return true;

        if (isInput())
        {
            FunctionalPin* upstream = getUpstreamFunctionalPin();
            if (!upstream)
                return false;

            return upstream->isPassThroughEnabled() ? upstream->dfsHasUpstreamKnownInterfaces() : true;
        }

        if (hasParentNode<GraphNode>())
        {
            GraphNode* parentGraph = dynamic_cast<GraphNode*>(getParentNode());
            std::vector<FunctionalPin*> inputPins;
            parentGraph->getInputFunctionalPins(GraphNode::InputPinQueryType::kConnectedAndPassThroughEnabled, &inputPins);

            for (FunctionalPin* inputPin : inputPins)
            {
                if (inputPin->dfsHasUpstreamKnownInterfaces())
                    return true;
            }

            return false;
        }

        return true;
    }

    mcd::FunctionalPin* FunctionalPin::getUpstreamFunctionalPin()
    {
        if (!isInput())
            return nullptr;

		std::vector<mcd::Pin*> connectedPins;
		getConnectedPins(connectedPins);

		if (connectedPins.empty())
            return nullptr;

		Pin* firstPin = connectedPins.front();

        if (firstPin->isOfType<FunctionalPin>())
            return firstPin->asFunctionalPin();
        else if (firstPin->isOfType<PassDownPin>())          
            return firstPin->asPassDownPin()->recurseUpstreamToFunctionalPin();

		return nullptr;
	}

    mcd::FunctionalPin* FunctionalPin::getDownstreamFunctionalPin()
    {
        if (isInput())
			return nullptr;

		std::vector<mcd::Pin*> connectedPins;
		getConnectedPins(connectedPins);

        if (connectedPins.empty())
			return nullptr;

		Pin* firstPin = connectedPins.front();

        if (firstPin->isOfType<FunctionalPin>())
            return firstPin->asFunctionalPin();
        else if (firstPin->isOfType<PassDownPin>())
			return firstPin->asPassDownPin()->getFirstDownstreamFunctionalPin();

		return nullptr;
    }
}