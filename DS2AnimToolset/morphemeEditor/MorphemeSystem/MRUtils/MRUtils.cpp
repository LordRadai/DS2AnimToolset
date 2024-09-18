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

	MR::NodeDef* getAnimNodeByAnimID(MR::NetworkDef* netDef, int assetId)
	{
		int numNodes = netDef->getNumNodeDefs();

		for (int idx = 0; idx < numNodes; idx++)
		{
			MR::NodeDef* node = netDef->getNodeDef(idx);

			if (node->getNodeTypeID() == NODE_TYPE_ANIM_EVENTS)
			{
				MR::AttribDataSourceAnim* source_anim = (MR::AttribDataSourceAnim*)node->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM);

				if (source_anim && (source_anim->m_animAssetID == assetId))
					return node;
			}
		}

		return nullptr;
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