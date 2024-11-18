#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrNodeDef.h"

namespace MD
{
	namespace Node
	{
		ME::NodeExportXML* exportBlend2Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportBlend2x2Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportBlendNNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportFeatherBlendNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportSubtractiveBlendNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
	}
}