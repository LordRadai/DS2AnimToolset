#include "MMDataPin.h"

namespace NodeEditor
{
	namespace Manifest
	{
		std::string MMDataPin::getDataType() const
		{
			nlohmann::json typeData = m_jsonData["type"];

			if (typeData.is_object())
				return typeData["default"];

			return typeData;
		}
	}
}