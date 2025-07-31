#include "MMDataPin.h"
#include "mcd/Pin/DataPin.h"

namespace mcc
{
	bool MMDataPin::addToGraphNode(mcd::GraphNode* node)
	{
		mcd::DataPin* pin = new mcd::DataPin(node, getPinName(), utils::getDataTypeFromString(this->getDataType()));
		pin->setPassThroughEnabled(isPassThrough());
		pin->setInput(isInput());

		node->getPins()->add(pin);
		return true;
	}
}
