#include "Pin.h"

namespace mcd
{
	Pin::Pin(db::Node* parent, const std::string& name, const std::string pinName)
		: db::Node(parent, name, pinName);
}
