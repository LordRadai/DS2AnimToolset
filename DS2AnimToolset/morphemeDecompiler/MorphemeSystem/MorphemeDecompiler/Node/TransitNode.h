#pragma once
#include "NodeExportBase.h"
#include "morpheme/Nodes/mrNodeTransit.h"
#include "morpheme/Nodes/mrNodeTransitSyncEvents.h"
#include "morpheme/mrAttribData.h"

namespace MD
{
	namespace Node
	{
		class NodeTransitDecompilerBase : public NodeDecompilerBase
		{
		public:
			NodeTransitDecompilerBase() {}
			~NodeTransitDecompilerBase() {}

		protected:
			void writeDeltaTrajSrouce(MR::NodeDef* nodeDef, ME::DataBlockExportXML* dataBlockExport);
		};

		class NodeTransitDecompiler : public NodeTransitDecompilerBase
		{
		public:
			NodeTransitDecompiler() {}
			~NodeTransitDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeTransitSyncEventsDecompiler : public NodeTransitDecompilerBase
		{
		public:
			NodeTransitSyncEventsDecompiler() {}
			~NodeTransitSyncEventsDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};
	}
}