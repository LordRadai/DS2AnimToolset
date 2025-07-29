#pragma once
#include "MMPin.h"
#include "DataTypes.h"

namespace mcc
{
	class MMDataPin : public MMPin
	{
		DataTypes m_dataType;

		DataTypes getDataTypeFromString(const std::string& typeStr) const
		{
			if (typeStr == "int") return DataTypes::kInt;
			else if (typeStr == "float") return DataTypes::kFloat;
			else if (typeStr == "string") return DataTypes::kString;
			else if (typeStr == "bool") return DataTypes::kBool;
			else if (typeStr == "vector3") return DataTypes::kVector3;
			else if (typeStr == "vector4") return DataTypes::kVector4;
			else if (typeStr == "quaternion") return DataTypes::kQuaternion;
			return DataTypes::kNumDataTypes;
		}
	public:
		MMDataPin() = default;
		virtual ~MMDataPin() override = default;
		virtual void fromJson(const nlohmann::json& json) override;
		DataTypes getDataType() const { return m_dataType; }
		void setDataType(DataTypes dataType) { m_dataType = dataType; }
	};
}
