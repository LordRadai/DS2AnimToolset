#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrNodeDef.h"
#include "NodeUtils.h"
#define THROW_NODE_TYPE_MISMATCH(nodeDef, type) if (nodeDef->getNodeTypeID() != type) { g_appLog->panicMessage("Expecting node type %d (got %d)\n", type, nodeDef->getNodeTypeID()); }

namespace MD
{
	namespace Node
	{
		ME::NodeExportXML* exportNodeCore(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportNodeUnhandled(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);

		ME::NodeExportXML* exportNetworkNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportAnimSyncEventsNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportMirrorTransformNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportFilterTransformsNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportSingleFrameNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportFreezeNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportEmitRequestOnDiscreteEventNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportPlaySpeedModiferNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportPassThroughNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportSmoothTransformsNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportSwitchNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportSequenceNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportScaleToDuration(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		ME::NodeExportXML* exportApplyGlobalTimeNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);

		void exportNodeTransitions(MR::NodeDef* nodeDef, ME::NodeExportXML* nodeExport);
	}
}