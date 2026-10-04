#include "MMStateMachineNode.h"

namespace NodeEditor
{
	namespace Manifest
	{
		StateNode* MMStateMachineNode::makeNode(Editor* editor, Graph* parent, int id, const std::string& name)
		{
			StateNode* node = new StateNode(editor, parent, id, name, nullptr);

			// Add pins
			std::vector<MMPin*> sortedPins = getSortedPins();
			for (auto& pin : sortedPins)
			{
				if (pin->isOfType<MMDataPin>())
				{
					MMDataPin* dataPin = pin->asType<MMDataPin>();
					DataPin::DataType dataType = DataPin::stringToDataType(dataPin->getDataType());

					if (dataPin->isInput())
						node->createInputDataPin(dataPin->getPinName(), dataType);
					else
						node->createOutputDataPin(dataPin->getPinName(), dataType);
				}

				else if (pin->isOfType<MMFunctionalPin>())
				{
					MMFunctionalPin* funcPin = pin->asType<MMFunctionalPin>();

					if (funcPin->isInput())
						node->createInputPin(funcPin->getPinName());
					else
						node->createOutputPin(funcPin->getPinName());
				}
			}

			// Add attributes
			for (uint32_t i = 0; i < getNumAttributes(); i++)
			{
				MMAttribute* mmAttr = getAttribute(i);

				node->addAttribute(mmAttr->makeAttribute(node));
			}

			return node;
		}
	}
}