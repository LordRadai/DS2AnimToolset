#pragma once
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "../TransitConditionExport/TransitConditionExport.h"

namespace MD
{
	namespace Node
	{
		ME::NodeExportXML* exportStateMachineNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	}
}