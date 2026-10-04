#include "RCore.h"

#include "ControlParameter.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	ControlParameter::ControlParameter(Editor* editor, int id, const std::string& name, ParameterType parameterType)
		: Entity(editor, name), m_controlParameterID(id), m_parameterType(parameterType)
	{
	}

	DataPin::DataType ControlParameter::getDataType() const
	{
		switch (m_parameterType)
		{
		case kParameterTypeFloat:			return DataPin::kDataTypeFloat;
		case kParameterTypeInt:				return DataPin::kDataTypeInt;
		case kParameterTypeUInt:			return DataPin::kDataTypeUInt;
		case kParameterTypeBool:			return DataPin::kDataTypeBool;
		case kParameterTypeVector3:			return DataPin::kDataTypeVector3;
		case kParameterTypeVector4:			return DataPin::kDataTypeVector4;
		case kParameterTypeQuaternion:		return DataPin::kDataTypeQuaternion;
		default:							INVOKE_PANIC("Invalid ControlParameter type %d", m_parameterType);
		}
	}

	const char* ControlParameter::parameterTypeToString(ParameterType type)
	{
		switch (type)
		{
		case kParameterTypeFloat:			return "float";
		case kParameterTypeInt:				return "int";
		case kParameterTypeUInt:			return "uint";
		case kParameterTypeBool:			return "bool";
		case kParameterTypeVector3:			return "vector3";
		case kParameterTypeVector4:			return "vector4";
		case kParameterTypeQuaternion:		return "quaternion";
		default:							return "unknown";
		}
	}

	ControlParameter::ParameterType ControlParameter::stringToParameterType(const std::string& typeStr)
	{
		if (typeStr == "float")				return kParameterTypeFloat;
		else if (typeStr == "int")			return kParameterTypeInt;
		else if (typeStr == "uint")			return kParameterTypeUInt;
		else if (typeStr == "bool")			return kParameterTypeBool;
		else if (typeStr == "vector3")		return kParameterTypeVector3;
		else if (typeStr == "vector4")		return kParameterTypeVector4;
		else if (typeStr == "quaternion")	return kParameterTypeQuaternion;

		INVOKE_PANIC("Invalid ControlParameter type string %s", typeStr.c_str());
	}
}
