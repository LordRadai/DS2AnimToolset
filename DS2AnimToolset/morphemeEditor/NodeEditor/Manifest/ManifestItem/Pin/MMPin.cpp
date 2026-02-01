#include "MMPin.h"

namespace Manifest
{
	void MMPin::fromJson(const nlohmann::json& json)
	{
		this->m_jsonData = json;
	}
}
