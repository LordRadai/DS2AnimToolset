#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrNodeDef.h"
#define THROW_NODE_TYPE_MISMATCH(nodeDef, type) if (nodeDef->getNodeTypeID() != type) { g_appLog->panicMessage("Expecting node type %d (got %d)\n", type, nodeDef->getNodeTypeID()); }

namespace MD
{
	namespace Node
	{
		ME::NodeExportXML* exportNodeCore(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportNodeUnhandled(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);

		ME::NodeExportXML* exportNetworkNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportAnimSyncEventsNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportMirrorTransformNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportFilterTransformsNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportSingleFrameNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportFreezeNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportEmitRequestOnDiscreteEventNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
		ME::NodeExportXML* exportPlaySpeedModiferNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);

		void exportNodeTransitions(MR::NodeDef* nodeDef, ME::NodeExportXML* nodeExport);

		void setNodeName(ME::NodeExportXML* node);
	}
}