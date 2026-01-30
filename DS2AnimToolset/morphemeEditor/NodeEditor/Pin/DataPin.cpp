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
}
