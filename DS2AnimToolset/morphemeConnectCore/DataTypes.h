#pragma once
#include <string>

enum class DataTypes
{
	kFloat,
	kVector3,
	kVector4,
	kBool,
	kQuaternion,
	kInt,
	kUInt,
	kString,

	kNumDataTypes
};

namespace utils
{
	DataTypes getDataTypeFromString(const std::string& typeName);
}