#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"

namespace MD
{
	namespace Node
	{
		ME::NodeExportXML* exportStateMachineNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
	}
}