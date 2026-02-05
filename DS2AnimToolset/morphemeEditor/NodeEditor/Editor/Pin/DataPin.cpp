#include "RCore.h"

#include "DataPin.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	void DataPin::draw()
	{
		unsigned int color = ImNodes::GetStyle().Colors[ImNodesCol_Pin];

		StyleSettings& style = m_ownerEditor->getStyleSettings();
		
		switch (m_dataType)
		{
		case kDataTypeFloat:		
			color = style.Colors[NodeEditorStyleCol_FloatDataPin];
			break;
		case kDataTypeVector3:		
			color = style.Colors[NodeEditorStyleCol_Vector3DataPin];
			break;
		case kDataTypeVector4:		
			color = style.Colors[NodeEditorStyleCol_Vector4DataPin];
			break;
		case kDataTypeBool:			
			color = style.Colors[NodeEditorStyleCol_BoolDataPin];
			break;
		case kDataTypeQuaternion:	
			color = style.Colors[NodeEditorStyleCol_QuaternionDataPin];
			break;
		case kDataTypeInt:			
			color = style.Colors[NodeEditorStyleCol_IntDataPin];
			break;
		case kDataTypeUInt:			
			color = style.Colors[NodeEditorStyleCol_UIntDataPin];
			break;
		default:
			break;
		}

		drawInternal(ImNodesPinShape_QuadFilled, color);
	}

	const char* DataPin::dataTypeToString(DataType dataType)
	{
		switch (dataType)
		{
		case kDataTypeFloat:		return "float";
		case kDataTypeVector3:		return "vector3";
		case kDataTypeVector4:		return "vector4";
		case kDataTypeBool:			return "bool";
		case kDataTypeQuaternion:	return "quaternion";
		case kDataTypeInt:			return "int";
		case kDataTypeUInt:			return "uint";
		default:					return "unknown";
		}
	}

	DataPin::DataType DataPin::stringToDataType(const std::string& typeStr)
	{
		if (typeStr == "float")
			return kDataTypeFloat;
		else if (typeStr == "vector3")
			return kDataTypeVector3;
		else if (typeStr == "vector4")
			return kDataTypeVector4;
		else if (typeStr == "bool")
			return kDataTypeBool;
		else if (typeStr == "quaternion")
			return kDataTypeQuaternion;
		else if (typeStr == "int")
			return kDataTypeInt;
		else if (typeStr == "uint")
			return kDataTypeUInt;

		INVOKE_PANIC("Invalid data type string %s",	typeStr.c_str());
	}
}
