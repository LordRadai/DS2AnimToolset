#pragma once
#include "NodeExportBase.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"

namespace MD
{
	namespace Node
	{
		class NodeStateMachineDecompiler : public NodeDecompilerBase
		{
		public:
			NodeStateMachineDecompiler() {}
			~NodeStateMachineDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeChildStates(MR::NodeDef* nodeDef, ME::DataBlockExportXML* nodeDataBlock);
			void writeActiveStateTransitions(MR::AttribDataStateMachineDef* stateMachineDef, ME::NodeExportXML* nodeExport);
		};
	}
}