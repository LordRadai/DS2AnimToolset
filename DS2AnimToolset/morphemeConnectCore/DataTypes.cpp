#include "DataTypes.h"

namespace utils
{
	DataTypes getDataTypeFromString(const std::string& typeName)
	{
		if (typeName == "float") return DataTypes::kFloat;
		if (typeName == "vector3") return DataTypes::kVector3;
		if (typeName == "vector4") return DataTypes::kVector4;
		if (typeName == "bool") return DataTypes::kBool;
		if (typeName == "quaternion") return DataTypes::kQuaternion;
		if (typeName == "int") return DataTypes::kInt;
		if (typeName == "uint") return DataTypes::kUInt;
		if (typeName == "string") return DataTypes::kString;
		return DataTypes::kNumDataTypes;
	}
}
