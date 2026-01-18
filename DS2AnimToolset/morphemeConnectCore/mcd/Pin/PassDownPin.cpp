#include "PassDownPin.h"
#include "mcd/Graph/Graph.h"
#include "mcu/Log/Log.h"

namespace mcd
{
	PassDownPin::PassDownPin(db::Node* parent, const std::string& name)
		: Pin(parent, "PassDownPin", name)
	{
		setReference(true);
	}

	mcd::Graph* PassDownPin::getChildGraph()
	{
		LOG_NOT_IMPLEMENTED();

		return nullptr;
	}
}