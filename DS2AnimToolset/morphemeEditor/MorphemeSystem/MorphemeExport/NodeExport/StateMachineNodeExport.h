#pragma once
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "../TransitConditionExport/TransitConditionExport.h"

namespace MorphemeExport
{
	namespace NodeExport
	{
		ME::NodeExportXML* exportStateMachineNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	}
}