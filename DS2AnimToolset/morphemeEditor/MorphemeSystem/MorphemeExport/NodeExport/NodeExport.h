#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrNodeDef.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeMirrorTransforms.h"

namespace MorphemeExport
{
	namespace NodeExport
	{
		ME::NodeExportXML* exportNodeCore(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportNodeUnhandled(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);

		ME::NodeExportXML* exportNetworkNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportAnimSyncEventsNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportMirrorTransformNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);

		void exportNodeTransitions(MR::NodeDef* nodeDef, ME::NodeExportXML* nodeExport);
	}
}