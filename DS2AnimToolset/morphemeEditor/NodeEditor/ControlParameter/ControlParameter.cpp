#include "ControlParameter.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	ControlParameter::ControlParameter(NodeEditor* editor, int id, const std::string& name, ParameterType parameterType)
		: Entity(editor, name), m_controlParameterID(id), m_parameterType(parameterType)
	{
		DataPin::DataType dataType;

		switch (parameterType)
		{
		case kParameterTypeFloat:
			dataType = DataPin::kDataTypeFloat;
			break;
		case kParameterTypeInt:
			dataType = DataPin::kDataTypeInt;
			break;
		case kParameterTypeUInt:
			dataType = DataPin::kDataTypeUInt;
			break;
		case kParameterTypeBool:
			dataType = DataPin::kDataTypeBool;
			break;
		case kParameterTypeVector3:
			dataType = DataPin::kDataTypeVector3;
			break;
		case kParameterTypeVector4:
			dataType = DataPin::kDataTypeVector4;
			break;
		case kParameterTypeQuaternion:
			dataType = DataPin::kDataTypeQuaternion;
			break;
		default:
			dataType = DataPin::kDataTypeFloat;
			break;
		}

		m_output = new DataPin(editor, editor->getControlParametersNode(), name, false, dataType);
	}

	ControlParameter::ControlParameter(NodeEditor* editor, Project::ProjectControlParameter* projectParameter) : ControlParameter(editor, projectParameter->getControlParameterID(), projectParameter->getName(), stringToParameterType(projectParameter->getType()))
	{
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

		throw std::invalid_argument("Invalid ControlParameter type string: " + typeStr);
	}
}
