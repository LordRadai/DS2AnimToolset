#pragma once
#include "morpheme/mrNodeDef.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/mrAttribData.h"
#include <vector>

namespace MRUtils
{
	MR::NodeDef* getParentStateMachineNode(MR::NodeDef* nodeDef);
	MR::StateDef* getStateDefReferencingCondition(int conditionIndex, MR::AttribDataStateMachineDef* stateMachineDef);
	MR::StateDef* getTargetNodeStateDef(short nodeID, MR::AttribDataStateMachineDef* stateMachineDef);
	MR::NodeDef* getAnimNodeByAnimID(MR::NetworkDef* netDef, int assetId);
	std::vector<MR::StateDef*> getStateMachineSteadyChildNodes(MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	std::vector<MR::StateDef*> getStateMachineTransitChildNodes(MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	int getConditionIndexByID(MR::StateDef* stateDef, int conditionID);
}