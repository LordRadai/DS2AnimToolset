#include "ControlParameter.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	ControlParameter::ControlParameter(NodeEditor* editor, const std::string& name, ParameterType parameterType)
		: Entity(editor, name), m_parameterType(parameterType)
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

	ControlParameter::ControlParameter(NodeEditor* editor, Project::ControlParameter* projectParameter) : Entity(editor, projectParameter)
	{
		const std::string typeStr = projectParameter->getType();

		if (typeStr == "float")
			m_parameterType = kParameterTypeFloat;
		else if (typeStr == "int")
			m_parameterType = kParameterTypeInt;
		else if (typeStr == "uint")
			m_parameterType = kParameterTypeUInt;
		else if (typeStr == "bool")
			m_parameterType = kParameterTypeBool;
		else if (typeStr == "vector3")
			m_parameterType = kParameterTypeVector3;
		else if (typeStr == "vector4")
			m_parameterType = kParameterTypeVector4;
		else if (typeStr == "quaternion")
			m_parameterType = kParameterTypeQuaternion;
		else
			m_parameterType = kParameterTypeFloat;

		DataPin::DataType dataType;

		switch (m_parameterType)
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

		m_output = new DataPin(editor, editor->getControlParametersNode(), getName(), false, dataType);
	}
}
