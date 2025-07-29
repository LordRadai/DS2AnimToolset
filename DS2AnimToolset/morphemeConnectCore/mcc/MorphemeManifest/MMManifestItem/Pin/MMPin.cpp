#include "MMPin.h"

namespace mcc
{
	void MMPin::fromJson(const nlohmann::json& json)
	{
		this->m_jsonData = json;
	}
}
