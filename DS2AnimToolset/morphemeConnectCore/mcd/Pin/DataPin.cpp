#include "DataPin.h"

namespace mcd
{
	DataPin::DataPin(db::Node* parent, const std::string pinName, const std::string& type)
		: Pin(parent, "DataPin", pinName),
		m_dataType(this, "Type", type)
	{
		this->addAttribute(&m_dataType);
	}
}
