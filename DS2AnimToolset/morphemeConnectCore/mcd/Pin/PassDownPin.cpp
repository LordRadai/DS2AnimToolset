#include "PassDownPin.h"
#include "mcd/Graph/Graph.h"

namespace mcd
{
	PassDownPin::PassDownPin(db::Node* parent, const std::string& name)
		: Pin(parent, "PassDownPin", name)
	{
		setReference(true);
	}

	mcd::Graph* PassDownPin::getChildGraph()
	{
		
		return nullptr;
	}
}