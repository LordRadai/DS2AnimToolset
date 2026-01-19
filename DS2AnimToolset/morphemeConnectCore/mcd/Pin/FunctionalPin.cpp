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
		if (m_interfaces->empty())
			addAttribute(m_interfaces.get());

		m_interfaces->add(interfaceName);
	}

	void FunctionalPin::removeInterface(const uint32_t index)
	{
		if (index < 0 || index >= m_interfaces->size())
			throw std::out_of_range("FunctionalPin::removeInterface() - Index out of range");

		m_interfaces->removeAt(index);

		if (m_interfaces->empty())
			removeAttribute(m_interfaces.get());
	}

	void FunctionalPin::setPassThroughEnabled(bool enabled)
	{
		removeAttribute(m_passThroughEnabled.get());

		// Only add to the list if it is enabled.
		if (enabled)
			addAttribute(m_passThroughEnabled.get());

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

	}
}