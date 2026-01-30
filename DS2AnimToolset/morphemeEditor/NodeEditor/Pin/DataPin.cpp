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
		case kDataTypeFloat:		color = style.Colors[NodeEditorStyleCol_FloatDataPin];
		case kDataTypeVector3:		color = style.Colors[NodeEditorStyleCol_Vector3DataPin];
		case kDataTypeVector4:		color = style.Colors[NodeEditorStyleCol_Vector4DataPin];
		case kDataTypeBool:			color = style.Colors[NodeEditorStyleCol_BoolDataPin];
		case kDataTypeQuaternion:	color = style.Colors[NodeEditorStyleCol_QuaternionDataPin];
		case kDataTypeInt:			color = style.Colors[NodeEditorStyleCol_IntDataPin];
		case kDataTypeUInt:			color = style.Colors[NodeEditorStyleCol_UIntDataPin];
		default:
			break;
		}

		drawInternal(ImNodesPinShape_QuadFilled, color);
	}
}
