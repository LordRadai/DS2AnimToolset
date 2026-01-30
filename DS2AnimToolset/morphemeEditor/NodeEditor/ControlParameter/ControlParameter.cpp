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
}
