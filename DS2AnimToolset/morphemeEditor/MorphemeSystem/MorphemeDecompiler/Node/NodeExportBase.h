#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrNodeDef.h"
#include "morpheme/mrNetworkDef.h"
#define THROW_NODE_TYPE_MISMATCH(nodeDef, type) if (nodeDef->getNodeTypeID() != type) { g_appLog->panicMessage("Expecting node type %d (got %d)\n", type, nodeDef->getNodeTypeID()); }

namespace MD
{
	namespace Node
	{
		class NodeDecompilerBase
		{
		public:
			NodeDecompilerBase() {}
			~NodeDecompilerBase() {}

			// TODO: Remove MR::NetworkDef* and ME::NetworkDefExportXML* since they are never used
			virtual ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
			void exportNodeTransitions(MR::NodeDef* nodeDef, ME::NodeExportXML* nodeExport);
		};
	}
}