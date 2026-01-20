#include "MMDataPin.h"
#include "mcd/Pin/DataPin.h"

namespace mcc
{
	bool MMDataPin::addToGraphNode(mcd::GraphNode* node)
	{
		mcd::DataPin* pin = new mcd::DataPin(node, getPinName(), mcd::DataPin::getDataTypeFromString(this->getDataType()));
		pin->setIsInput(isInput());

		node->getPins()->add(pin);
		return true;
	}
}
