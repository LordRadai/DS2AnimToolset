#pragma once
#include "morpheme/mrNodeDef.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/mrAttribData.h"

namespace MRUtils
{
	MR::NodeDef* getParentStateMachineNode(MR::NodeDef* nodeDef);
	MR::StateDef* getTargetNodeStateDef(short nodeID, MR::AttribDataStateMachineDef* stateMachineDef);
	int getConditionIndexByID(MR::StateDef* stateDef, int conditionID);
}