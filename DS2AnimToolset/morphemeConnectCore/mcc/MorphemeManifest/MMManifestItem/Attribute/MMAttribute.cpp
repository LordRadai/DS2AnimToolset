#include "MMAttribute.h"
#include "mcd/Attribute/SingleValue/FloatAttribute.h"


namespace mcc
{
	void MMAttribute::fromJson(const nlohmann::json& json)
	{
		this->m_jsonData = json;
	}
}