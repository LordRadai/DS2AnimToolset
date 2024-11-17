#include "MorphemeUtils.h"

namespace MorphemeUtils
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

	MR::StateDef* getStateDefReferencingCondition(int conditionIndex, MR::AttribDataStateMachineDef* stateMachineDef)
	{
		for (int i = 0; i < stateMachineDef->getNumStates(); i++)
		{
			MR::StateDef* stateDef = stateMachineDef->getStateDef(i);

			for (size_t j = 0; j < stateDef->getNumEntryConditions(); j++)
			{
				if (stateDef->getEntryConditionStateMachineIndex(j) == conditionIndex)
					return stateDef;
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

	std::vector<MR::StateDef*> getStateMachineSteadyChildNodes(MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
	{
		std::vector<MR::StateDef*> childStates;

		MR::AttribDataStateMachineDef* attribData = static_cast<MR::AttribDataStateMachineDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

		if (attribData == nullptr)
			return childStates;

		int childStateCount = nodeDef->getNumChildNodes();

		for (size_t i = 0; i < childStateCount; i++)
		{
			MR::StateDef* stateDef = attribData->getStateDef(i);

			int nodeID = stateDef->getNodeID();

			MR::NodeDef* def = netDef->getNodeDef(nodeID);

			int nodeType = def->getNodeTypeID();

			if ((nodeType != NODE_TYPE_TRANSIT) && (nodeType != NODE_TYPE_TRANSIT_PHYSICS) && (nodeType != NODE_TYPE_TRANSIT_SYNC_EVENTS) && (nodeType != NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS))
				childStates.push_back(stateDef);
		}

		return childStates;
	}

	std::vector<MR::StateDef*> getStateMachineTransitChildNodes(MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
	{
		std::vector<MR::StateDef*> childStates;

		MR::AttribDataStateMachineDef* attribData = static_cast<MR::AttribDataStateMachineDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

		if (attribData == nullptr)
			return childStates;

		int childStateCount = nodeDef->getNumChildNodes();

		for (size_t i = 0; i < childStateCount; i++)
		{
			MR::StateDef* stateDef = attribData->getStateDef(i);

			int nodeID = stateDef->getNodeID();

			MR::NodeDef* def = netDef->getNodeDef(nodeID);

			int nodeType = def->getNodeTypeID();

			if ((nodeType == NODE_TYPE_TRANSIT) || (nodeType == NODE_TYPE_TRANSIT_PHYSICS) || (nodeType == NODE_TYPE_TRANSIT_SYNC_EVENTS) || (nodeType == NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS))
				childStates.push_back(stateDef);
		}

		return childStates;
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

	const char* getNodeTypeName(MR::NodeType typeID)
	{
        NODE_TYPE_NETWORK;
        switch (typeID) {
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 9): return "Network";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 10): return "StateMachine";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 20): return "CpFloat";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 21): return "CpVector3";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 22): return "CpVector4";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 23): return "CpBool";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 24): return "CpInt";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 25): return "CpUint";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 26): return "CpPhysicsObjectPointer";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 103): return "Animation";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 104): return "AnimEvents";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 105): return "FilterTransforms";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 107): return "Blend2";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 108): return "BlendN";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 109): return "Singleframe";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 126): return "Freeze";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 149): return "Blend2x2";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 147): return "BlendNxm";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 169): return "BlendAll";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 170): return "SubtractiveBlend";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 180): return "ScatterBlend1D";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 181): return "ScatterBlend2D";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 182): return "ScatterBlend3D";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 102): return "CpOpBoolean";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 110): return "CpOpFunction";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 111): return "CpOpArithmetic";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 184): return "CpOpArithmeticVector3";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 112): return "CpOpOneInputArithmetic";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 185): return "CpOpOneInputArithmeticVector3";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 113): return "CpOpNoiseGen";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 114): return "FeatherBlend2";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 115): return "ApplyBindPose";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 116): return "PhysicsGrouper";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 117): return "BehaviourGrouper";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 119): return "ApplyGlobalTime";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 124): return "ApplyPhysicsJointLimits";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 127): return "SetNonPhysicsTransforms";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 120): return "TwoBoneIk";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 121): return "LockFoot";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 122): return "HeadLook";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 123): return "Physics";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 128): return "Behaviour";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 129): return "HipsIk";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 183): return "PerformanceBehaviour";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 125): return "PlaySpeedModifier";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 151): return "ScaleToDuration";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 152): return "EmitJointCpInfo";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 153): return "EmitMessageOnDiscreteEvent";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 130): return "ClosestAnim";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 131): return "Switch";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 133): return "Sequence";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 134): return "Passthrough";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 135): return "MirrorTransforms";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 136): return "BasicUnevenTerrain";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 137): return "ActiveState";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 138): return "PredictiveUnevenTerrain";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 139): return "CpOpIntToFloat";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 141): return "CpOpRayCast";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 142): return "CpOpSmoothFloat";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 143): return "CpOpRampFloat";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 144): return "CpOpFloatsToVector3";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 145): return "CpOpFloatToInt";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 146): return "CpOpRandomFloat";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 148): return "CpOpPhysicsInfo";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 155): return "CpOpConvertToCharacterSpace";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 154): return "CpOpVector3ToFloats";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 164): return "CpOpVector3Dot";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 163): return "CpOpOrientationInFreefall";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 165): return "CpOpHit";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 166): return "CpOpRollDownStairs";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 168): return "CpOpApplyImpulse";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 167): return "CpOpContactReporter";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 171): return "CpOpVector3Distance";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 172): return "CpOpVector3CrossProduct";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 173): return "CpOpRateOfChange";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 174): return "CpOpVector3Angle";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 175): return "CpOpVector3Normalise";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 176): return "CpOpPhysicalConstraint";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 177): return "CpOpFallOverWall";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 161): return "Retarget";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 162): return "ScaleCharacter";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 150): return "GunAimIk";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 400): return "TransitSyncEvents";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 401): return "TransitSyncEventsPhysics";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 402): return "Transit";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 403): return "TransitPhysics";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 178): return "ModifyJointTransform";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 179): return "ModifyTrajectoryTransform";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 500): return "SmoothTransforms";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 501): return "CpOpEmitMessageAndCp";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 502): return "TrajectoryOverride";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 503): return "CcOverride";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 504): return "LimbInfo";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 505): return "ExpandLimits";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 200): return "CpOpCondOnMessage";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 201): return "CpOpCondCpCompare";
            case GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 202): return "CpOpTimeLag";
            default: return "InvalidTypeID";
        }
	}
}