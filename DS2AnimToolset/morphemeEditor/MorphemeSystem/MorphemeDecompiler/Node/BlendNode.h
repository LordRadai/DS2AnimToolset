#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrNodeDef.h"
#include "morpheme/Nodes/mrNodeFeatherBlend2.h"
#include "morpheme/Nodes/mrNodeFeatherBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"

namespace MD
{
	namespace Node
	{
		ME::NodeExportXML* exportBlend2Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportBlendNNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportFeatherBlend2Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	}
}