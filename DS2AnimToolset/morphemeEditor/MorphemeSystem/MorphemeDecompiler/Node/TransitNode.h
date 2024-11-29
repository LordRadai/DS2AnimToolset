#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/Nodes/mrNodeTransit.h"
#include "morpheme/Nodes/mrNodeTransitSyncEvents.h"
#include "morpheme/mrAttribData.h"

namespace MD
{
	namespace Node
	{
		ME::NodeExportXML* exportTransitNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportTransitSyncEventsNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
	}
}