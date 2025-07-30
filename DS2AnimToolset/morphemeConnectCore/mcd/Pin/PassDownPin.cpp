#include "PassDownPin.h"

namespace mcd
{
	PassDownPin::PassDownPin(db::Node* parent, const std::string& name)
		: Pin(parent, "PassDownPin", name)
	{
		setReference(true);
	}
}