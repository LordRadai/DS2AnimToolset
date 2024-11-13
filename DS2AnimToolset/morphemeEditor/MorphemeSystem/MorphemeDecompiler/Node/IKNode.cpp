#include "IKNode.h"
#include "Node.h"
#include "extern.h"
#include "RCore.h"
#include "morpheme/mrAttribData.h"
#include "NMIK/NMPointIK.h"

namespace MD
{
	namespace Node
	{
		uint32_t getUpAxisIndex(NMP::Vector3 upAxisVector)
		{
			uint32_t upAxisIndex = 1;

			for (BYTE i = 0; i < 3; i++)
			{
				if (upAxisVector[i] == 1.f)
					return i;
			}

			g_appLog->panicMessage("Invalid up axis vector (%.3f, %.3f, %.3f)\n", upAxisVector[0], upAxisVector[1], upAxisVector[2]);
			return -1;
		}

		ME::NodeExportXML* exportHeadLookNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_HEAD_LOOK);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Target", nodeDef->getInputCPConnection(0));
			NodeUtils::writeInputCPConnection(nodeDataBlock, "BlendWeight", nodeDef->getInputCPConnection(1));

			MR::AttribDataHeadLookSetup* headLookSetup = static_cast<MR::AttribDataHeadLookSetup*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));
			
			nodeDataBlock->writeBool(headLookSetup->m_updateTargetByDeltas, "UpdateTargetByDeltas");
			nodeDataBlock->writeBool(headLookSetup->m_worldSpaceTarget, "WorldSpaceTarget");

			CHAR paramName[256];
			for (int animSetIndex = 0; animSetIndex < netDef->getNumAnimSets(); animSetIndex++)
			{
				MR::AttribDataHeadLookChain* headLookChain = static_cast<MR::AttribDataHeadLookChain*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET));

				if (animSetIndex == 0)
				{
					nodeDataBlock->writeBool(true, "ApplyJointLimits");
					nodeDataBlock->writeBool(headLookChain->m_ikParams->minimiseRotation, "MinimiseRotation");
					nodeDataBlock->writeBool(headLookChain->m_ikParams->keepUpright, "KeepUpright");

					nodeDataBlock->writeFloat(headLookChain->m_ikParams->worldUpAxis.x, "WorldUpAxisX");
					nodeDataBlock->writeFloat(headLookChain->m_ikParams->worldUpAxis.y, "WorldUpAxisY");
					nodeDataBlock->writeFloat(headLookChain->m_ikParams->worldUpAxis.z, "WorldUpAxisZ");
				}

				sprintf_s(paramName, "PointingVectorX_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(headLookChain->m_ikParams->pointingVector.x, paramName);
				sprintf_s(paramName, "PointingVectorY_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(headLookChain->m_ikParams->pointingVector.y, paramName);
				sprintf_s(paramName, "PointingVectorZ_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(headLookChain->m_ikParams->pointingVector.z, paramName);

				sprintf_s(paramName, "EndEffectorOffsetX_%d", animSetIndex + 1);
				nodeDataBlock->readFloat(headLookChain->m_endEffectorOffset.x, paramName);
				sprintf_s(paramName, "EndEffectorOffsetY_%d", animSetIndex + 1);
				nodeDataBlock->readFloat(headLookChain->m_endEffectorOffset.y, paramName);
				sprintf_s(paramName, "EndEffectorOffsetZ_%d", animSetIndex + 1);
				nodeDataBlock->readFloat(headLookChain->m_endEffectorOffset.z, paramName);

				sprintf_s(paramName, "Bias_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(headLookChain->m_ikParams->bias, paramName);

				sprintf_s(paramName, "EndJointIndex_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(headLookChain->m_endJointIndex, paramName);

				sprintf_s(paramName, "RootJointIndex_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(headLookChain->m_rootJointIndex, paramName);
			}

			return nodeExportXML;
		}

		ME::NodeExportXML* exportHipsIKNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_HIPS_IK);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "SourceNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "PositionDelta", nodeDef->getInputCPConnection(0));
			NodeUtils::writeInputCPConnection(nodeDataBlock, "RotationDeltaQuatNodeID", nodeDef->getInputCPConnection(1));
			NodeUtils::writeInputCPConnection(nodeDataBlock, "RotationDeltaEulerNodeID", nodeDef->getInputCPConnection(2));
			NodeUtils::writeInputCPConnection(nodeDataBlock, "FootTurnWeight", nodeDef->getInputCPConnection(3));
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Weight", nodeDef->getInputCPConnection(4));

			MR::AttribDataHipsIKDef* hipsIKDef = static_cast<MR::AttribDataHipsIKDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			uint32_t upAxisIndex = getUpAxisIndex(hipsIKDef->m_upAxis);
			nodeDataBlock->writeUInt(upAxisIndex, "UpAxisIndex");
			nodeDataBlock->writeBool(hipsIKDef->m_keepWorldFootOrientation, "KeepWorldFootOrientation");
			nodeDataBlock->writeFloat(hipsIKDef->m_swivelOrientationWeight, "KneeSwivelWeight");
			nodeDataBlock->writeBool(hipsIKDef->m_localReferenceFrame, "LocalReferenceFrame");

			CHAR paramName[256];
			for (uint32_t animSetIdx = 0; animSetIdx < numAnimSets; animSetIdx++)
			{
				MR::AttribDataHipsIKAnimSetDef* hipsIKAnimSetDef = static_cast<MR::AttribDataHipsIKAnimSetDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET));

				sprintf_s(paramName, "HipsIndex_%d", animSetIdx + 1);
				nodeDataBlock->writeInt(hipsIKAnimSetDef->m_hipsIndex, paramName);

				sprintf_s(paramName, "LeftKneeRotationAxisX_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_leftKneeRotationAxis.x, paramName);
				sprintf_s(paramName, "LeftKneeRotationAxisY_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_leftKneeRotationAxis.y, paramName);
				sprintf_s(paramName, "LeftKneeRotationAxisZ_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_leftKneeRotationAxis.z, paramName);

				sprintf_s(paramName, "RightKneeRotationAxisX_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_rightKneeRotationAxis.x, paramName);
				sprintf_s(paramName, "RightKneeRotationAxisY_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_rightKneeRotationAxis.y, paramName);
				sprintf_s(paramName, "RightKneeRotationAxisZ_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_rightKneeRotationAxis.z, paramName);

				sprintf_s(paramName, "UseBallJoints_%d", animSetIdx + 1);
				nodeDataBlock->writeBool(hipsIKAnimSetDef->m_useBallJoints, paramName);

				if (hipsIKAnimSetDef->m_useBallJoints)
				{
					sprintf_s(paramName, "LeftBallIndex_%d", animSetIdx + 1);
					nodeDataBlock->writeInt(hipsIKAnimSetDef->m_leftLegIndex[3], paramName);
				}
				else
				{
					sprintf_s(paramName, "LeftAnkleIndex_%d", animSetIdx + 1);
					nodeDataBlock->writeInt(hipsIKAnimSetDef->m_leftLegIndex[2], paramName);
				}

				if (hipsIKAnimSetDef->m_useBallJoints)
				{
					sprintf_s(paramName, "RightBallIndex_%d", animSetIdx + 1);
					nodeDataBlock->writeInt(hipsIKAnimSetDef->m_rightLegIndex[3], paramName);
				}
				else
				{
					sprintf_s(paramName, "RightAnkleIndex_%d", animSetIdx + 1);
					nodeDataBlock->writeInt(hipsIKAnimSetDef->m_rightLegIndex[2], paramName);
				}
			}

			return nodeExportXML;
		}

		ME::NodeExportXML* exportLockFootNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_LOCK_FOOT);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "IkFkBlendWeight", nodeDef->getInputCPConnection(0));
			NodeUtils::writeInputCPConnection(nodeDataBlock, "SwivelContributionToOrientation", nodeDef->getInputCPConnection(1));
		
			MR::AttribDataLockFootSetup* lockFootSetup = static_cast<MR::AttribDataLockFootSetup*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeBool(lockFootSetup->m_assumeSimpleHierarchy, "AssumeSimpleHierarchy");
			nodeDataBlock->writeUInt(lockFootSetup->m_upAxis, "UpAxisIndex");
			nodeDataBlock->writeBool(lockFootSetup->m_lockVerticalMotion, "LockVerticalMotion");
			nodeDataBlock->writeBool(lockFootSetup->m_trackCharacterController, "TrackCharacterController");

			CHAR paramName[256];
			for (uint32_t animSetIdx = 0; animSetIdx < numAnimSets; animSetIdx++)
			{
				MR::AttribDataLockFootChain* hipsIKAnimSetDef = static_cast<MR::AttribDataLockFootChain*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET));

				sprintf_s(paramName, "FlipKneeRotationDirection_%d", animSetIdx + 1);
				nodeDataBlock->writeBool(false, paramName);

				sprintf_s(paramName, "KneeRotationAxisX_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_kneeRotationAxis.x, paramName);
				sprintf_s(paramName, "KneeRotationAxisY_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_kneeRotationAxis.y, paramName);
				sprintf_s(paramName, "KneeRotationAxisZ_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_kneeRotationAxis.z, paramName);

				sprintf_s(paramName, "BallRotationAxisX_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_ballRotationAxis.x, paramName);
				sprintf_s(paramName, "BallRotationAxisY_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_ballRotationAxis.y, paramName);
				sprintf_s(paramName, "BallRotationAxisZ_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_ballRotationAxis.z, paramName);

				sprintf_s(paramName, "FootLevelVectorX_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_ballRotationAxis.x, paramName);
				sprintf_s(paramName, "FootLevelVectorY_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_ballRotationAxis.y, paramName);
				sprintf_s(paramName, "FootLevelVectorZ_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_ballRotationAxis.z, paramName);

				sprintf_s(paramName, "UseBallJoint_%d", animSetIdx + 1);
				nodeDataBlock->writeBool(hipsIKAnimSetDef->m_useBallJoint, paramName);

				sprintf_s(paramName, "FixToeGroundPenetration_%d", animSetIdx + 1);
				nodeDataBlock->writeBool(hipsIKAnimSetDef->m_fixToeGroundPenetration, paramName);

				sprintf_s(paramName, "FixGroundPenetration_%d", animSetIdx + 1);
				nodeDataBlock->writeBool(hipsIKAnimSetDef->m_fixGroundPenetration, paramName);

				sprintf_s(paramName, "FixFootOrientation_%d", animSetIdx + 1);
				nodeDataBlock->writeBool(hipsIKAnimSetDef->m_fixFootOrientation, paramName);

				sprintf_s(paramName, "CatchUpSpeedFactor_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_catchUpSpeedFactor, paramName);

				sprintf_s(paramName, "StraightestLegFactor_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_straightestLegFactor, paramName);

				sprintf_s(paramName, "SnapToSourceDistance_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_snapToSourceDistance, paramName);

				sprintf_s(paramName, "AnkleLowerHeightBound_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_ankleLowerHeightBound, paramName);

				sprintf_s(paramName, "LowerHeightBound_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_lowerHeightBound, paramName);

				sprintf_s(paramName, "ToeLowerHeightBound_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_toeLowerHeightBound, paramName);

				sprintf_s(paramName, "ToeIndex_%d", animSetIdx + 1);
				nodeDataBlock->writeUInt(hipsIKAnimSetDef->m_jointIndex[4], paramName);

				sprintf_s(paramName, "BallIndex_%d", animSetIdx + 1);
				nodeDataBlock->writeUInt(hipsIKAnimSetDef->m_jointIndex[3], paramName);

				sprintf_s(paramName, "AnkleIndex_%d", animSetIdx + 1);
				nodeDataBlock->writeUInt(hipsIKAnimSetDef->m_jointIndex[2], paramName);

				sprintf_s(paramName, "FootfallEventID_%d", animSetIdx + 1);
				nodeDataBlock->writeUInt(hipsIKAnimSetDef->m_footfallEventID, paramName);
			}

			return nodeExportXML;
		}
	}
}