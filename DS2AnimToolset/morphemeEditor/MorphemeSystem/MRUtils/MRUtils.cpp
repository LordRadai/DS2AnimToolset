#include "MRUtils.h"

namespace MRUtils
{
	MR::NodeDef* getParentStateMachineNode(MR::NodeDef* nodeDef)
	{
		MR::NodeDef* parent = nodeDef->getParentNodeDef();

		while (true)
		{
			if (parent == nullptr || (parent->getNodeTypeID() == NODE_TYPE_STATE_MACHINE))
				return parent;

			parent = parent->getParentNodeDef();
		}
	}

	MR::StateDef* getTargetNodeStateDef(short nodeID, MR::AttribDataStateMachineDef* stateMachineDef)
	{
		for (int i = 0; i < stateMachineDef->getNumStates(); i++)
		{
			MR::StateDef* stateDef = stateMachineDef->getStateDef(i);

			if (stateDef->getNodeID() == nodeID)
				return stateDef;
		}

		return nullptr;
	}

	int getConditionIndexByID(MR::StateDef* stateDef, int conditionID)
	{
		for (size_t i = 0; i < stateDef->getNumExitConditions(); i++)
		{
			if (stateDef->getExitConditionStateMachineIndex(i) == conditionID)
				return i;
		}

		return -1;
	}
}