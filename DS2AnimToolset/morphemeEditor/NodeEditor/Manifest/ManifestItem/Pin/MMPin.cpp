#include "MMPin.h"

namespace NodeEditor
{
	namespace Manifest
	{
		void MMPin::fromJson(const nlohmann::json& json)
		{
			this->m_jsonData = json;
		}
	}
}