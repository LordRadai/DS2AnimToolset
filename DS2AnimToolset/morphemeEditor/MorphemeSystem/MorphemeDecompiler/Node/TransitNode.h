#pragma once
#include "NodeExportBase.h"
#include "morpheme/Nodes/mrNodeTransit.h"
#include "morpheme/Nodes/mrNodeTransitSyncEvents.h"
#include "morpheme/mrAttribData.h"

namespace MD
{
	namespace Node
	{
		class TransitNodeExportBase : public NodeExportBase
		{
		public:
			TransitNodeExportBase() {}
			~TransitNodeExportBase() {}

		protected:
			void writeDeltaTrajSrouce(MR::NodeDef* nodeDef, ME::DataBlockExportXML* dataBlockExport);
		};

		class NodeExportTransit : public TransitNodeExportBase
		{
		public:
			NodeExportTransit() {}
			~NodeExportTransit() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportTransitSyncEvents : public TransitNodeExportBase
		{
		public:
			NodeExportTransitSyncEvents() {}
			~NodeExportTransitSyncEvents() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};
	}
}