#include "IKNode.h"
#include "extern.h"
#include "RCore.h"
#include "morpheme/mrAttribData.h"
#include "NodeUtils.h"

namespace MD
{
	namespace Node
	{
		bool IKNodeExportBase::isAxisFlipped(NMP::Vector3 vec)
		{
			if ((vec.x < 0) || (vec.y < 0) || (vec.z < 0))
				return true;

			return false;
		}

		float IKNodeExportBase::getAngleFromClampedCosine(float in)
		{
			float cos = NMP::clampValue(in, 0.f, 1.f);
			float angleRad = acosf(cos);

			return NMP::radiansToDegrees(angleRad);
		}

		uint32_t IKNodeExportBase::getUpAxisIndex(NMP::Vector3 upAxisVector)
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

		float IKNodeExportBase::getBias(NMRU::IKJointParams* jointParams, int numJointParams)
		{
			if (numJointParams <= 1)
				return 1.f;

			const float ratio = 1.f / float(numJointParams - 1);

			return jointParams[1].weight * (numJointParams / (2 * ratio));
		}

		ME::NodeExportXML* NodeExportHeadLook::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_HEAD_LOOK);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Target", nodeDef->getInputCPConnection(0), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "BlendWeight", nodeDef->getInputCPConnection(1), true);

			MR::AttribDataHeadLookSetup* headLookSetup = static_cast<MR::AttribDataHeadLookSetup*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));
			
			nodeDataBlock->writeBool(headLookSetup->m_updateTargetByDeltas, "UpdateTargetByDeltas");
			nodeDataBlock->writeBool(headLookSetup->m_worldSpaceTarget, "WorldSpaceTarget");

			CHAR paramName[256];
			for (uint32_t animSetIndex = 0; animSetIndex < netDef->getNumAnimSets(); animSetIndex++)
			{
				MR::AttribDataHeadLookChain* headLookChain = static_cast<MR::AttribDataHeadLookChain*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, animSetIndex));

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
				nodeDataBlock->writeFloat(headLookChain->m_endEffectorOffset.x, paramName);
				sprintf_s(paramName, "EndEffectorOffsetY_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(headLookChain->m_endEffectorOffset.y, paramName);
				sprintf_s(paramName, "EndEffectorOffsetZ_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(headLookChain->m_endEffectorOffset.z, paramName);

				float bias = getBias(headLookChain->m_ikParams->perJointParams, headLookChain->m_numJoints);

				sprintf_s(paramName, "Bias_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(bias, paramName);

				sprintf_s(paramName, "EndJointIndex_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(headLookChain->m_endJointIndex, paramName);

				sprintf_s(paramName, "RootJointIndex_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(headLookChain->m_rootJointIndex, paramName);
			}

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeExportHipsIK::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_HIPS_IK);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "SourceNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "PositionDelta", nodeDef->getInputCPConnection(0), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "RotationDeltaQuatNodeID", nodeDef->getInputCPConnection(1), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "RotationDeltaEulerNodeID", nodeDef->getInputCPConnection(2), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "FootTurnWeight", nodeDef->getInputCPConnection(3), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Weight", nodeDef->getInputCPConnection(4), true);

			MR::AttribDataHipsIKDef* hipsIKDef = static_cast<MR::AttribDataHipsIKDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			uint32_t upAxisIndex = getUpAxisIndex(hipsIKDef->m_upAxis);
			nodeDataBlock->writeUInt(upAxisIndex, "UpAxisIndex");
			nodeDataBlock->writeFloat(hipsIKDef->m_swivelOrientationWeight, "KneeSwivelWeight");
			nodeDataBlock->writeBool(hipsIKDef->m_localReferenceFrame, "LocalReferenceFrame");
			nodeDataBlock->writeBool(hipsIKDef->m_keepWorldFootOrientation, "KeepWorldFootOrientation");

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

		ME::NodeExportXML* NodeExportLockFoot::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_LOCK_FOOT);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "IkFkBlendWeight", nodeDef->getInputCPConnection(0), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "SwivelContributionToOrientation", nodeDef->getInputCPConnection(1), true);
		
			MR::AttribDataLockFootSetup* lockFootSetup = static_cast<MR::AttribDataLockFootSetup*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeBool(lockFootSetup->m_assumeSimpleHierarchy, "AssumeSimpleHierarchy");
			nodeDataBlock->writeUInt(lockFootSetup->m_upAxis, "UpAxisIndex");
			nodeDataBlock->writeBool(lockFootSetup->m_lockVerticalMotion, "LockVerticalMotion");
			nodeDataBlock->writeBool(lockFootSetup->m_trackCharacterController, "TrackCharacterController");

			CHAR paramName[256];
			for (uint32_t animSetIdx = 0; animSetIdx < numAnimSets; animSetIdx++)
			{
				MR::AttribDataLockFootChain* hipsIKAnimSetDef = static_cast<MR::AttribDataLockFootChain*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, animSetIdx));

				const bool flippedAxis = isAxisFlipped(hipsIKAnimSetDef->m_kneeRotationAxis);

				sprintf_s(paramName, "FlipKneeRotationDirection_%d", animSetIdx + 1);
				nodeDataBlock->writeBool(flippedAxis, paramName);

				if (flippedAxis)
				{
					sprintf_s(paramName, "KneeRotationAxisX_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(-hipsIKAnimSetDef->m_kneeRotationAxis.x, paramName);
					sprintf_s(paramName, "KneeRotationAxisY_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(-hipsIKAnimSetDef->m_kneeRotationAxis.y, paramName);
					sprintf_s(paramName, "KneeRotationAxisZ_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(-hipsIKAnimSetDef->m_kneeRotationAxis.z, paramName);
				}
				else
				{
					sprintf_s(paramName, "KneeRotationAxisX_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_kneeRotationAxis.x, paramName);
					sprintf_s(paramName, "KneeRotationAxisY_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_kneeRotationAxis.y, paramName);
					sprintf_s(paramName, "KneeRotationAxisZ_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_kneeRotationAxis.z, paramName);
				}

				sprintf_s(paramName, "BallRotationAxisX_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_ballRotationAxis.x, paramName);
				sprintf_s(paramName, "BallRotationAxisY_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_ballRotationAxis.y, paramName);
				sprintf_s(paramName, "BallRotationAxisZ_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_ballRotationAxis.z, paramName);

				sprintf_s(paramName, "FootLevelVectorX_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_footLevelVector.x, paramName);
				sprintf_s(paramName, "FootLevelVectorY_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_footLevelVector.y, paramName);
				sprintf_s(paramName, "FootLevelVectorZ_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(hipsIKAnimSetDef->m_footLevelVector.z, paramName);

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

		ME::NodeExportXML* NodeExportGunAimIK::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_GUN_AIM_IK);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Target", nodeDef->getInputCPConnection(0), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "BlendWeight", nodeDef->getInputCPConnection(1), true);

			const MR::AttribDataGunAimSetup* gunAimSetupAttrib = static_cast<MR::AttribDataGunAimSetup*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeFloat(gunAimSetupAttrib->m_worldUpAxis.x, "WorldUpAxisX");
			nodeDataBlock->writeFloat(gunAimSetupAttrib->m_worldUpAxis.y, "WorldUpAxisY");
			nodeDataBlock->writeFloat(gunAimSetupAttrib->m_worldUpAxis.z, "WorldUpAxisZ");

			nodeDataBlock->writeBool(gunAimSetupAttrib->m_keepUpright, "KeepUpright");
			nodeDataBlock->writeBool(gunAimSetupAttrib->m_applyJointLimits, "ApplyJointLimits");
			nodeDataBlock->writeBool(gunAimSetupAttrib->m_worldSpaceTarget, "WorldSpaceTarget");

			CHAR paramName[256];
			for (uint32_t animSetIdx = 0; animSetIdx < numAnimSets; animSetIdx++)
			{
				const MR::AttribDataGunAimIKChain* gunAimChainAttrib = static_cast<MR::AttribDataGunAimIKChain*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, animSetIdx));

				sprintf_s(paramName, "GunJointIndex_%d", animSetIdx + 1);
				nodeDataBlock->writeUInt(gunAimChainAttrib->m_gunJointIndex, paramName);

				sprintf_s(paramName, "GunBindJointIndex_%d", animSetIdx + 1);
				nodeDataBlock->writeUInt(gunAimChainAttrib->m_gunBindJointIndex, paramName);

				sprintf_s(paramName, "GunPivotOffsetX_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(gunAimChainAttrib->m_gunPivotOffset.x, paramName);
				sprintf_s(paramName, "GunPivotOffsetY_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(gunAimChainAttrib->m_gunPivotOffset.y, paramName);
				sprintf_s(paramName, "GunPivotOffsetZ_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(gunAimChainAttrib->m_gunPivotOffset.z, paramName);

				sprintf_s(paramName, "GunBarrelOffsetX_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(gunAimChainAttrib->m_gunBarrelOffset.x, paramName);
				sprintf_s(paramName, "GunBarrelOffsetY_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(gunAimChainAttrib->m_gunBarrelOffset.y, paramName);
				sprintf_s(paramName, "GunBarrelOffsetZ_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(gunAimChainAttrib->m_gunBarrelOffset.z, paramName);

				sprintf_s(paramName, "GunPointingVectorX_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(gunAimChainAttrib->m_gunPointingVector.x, paramName);
				sprintf_s(paramName, "GunPointingVectorY_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(gunAimChainAttrib->m_gunPointingVector.y, paramName);
				sprintf_s(paramName, "GunPointingVectorZ_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(gunAimChainAttrib->m_gunPointingVector.z, paramName);

				sprintf_s(paramName, "PrimaryWristJointIndex_%d", animSetIdx + 1);
				nodeDataBlock->writeUInt(gunAimChainAttrib->m_primaryWristJointIndex, paramName);

				const bool primaryHingeFlipped = isAxisFlipped(gunAimChainAttrib->m_primaryArmHingeAxis);

				sprintf_s(paramName, "FlipPrimaryHinge_%d", animSetIdx + 1);
				nodeDataBlock->writeBool(primaryHingeFlipped, paramName);

				if (primaryHingeFlipped)
				{
					sprintf_s(paramName, "PrimaryArmHingeAxisX_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(-gunAimChainAttrib->m_primaryArmHingeAxis.x, paramName);
					sprintf_s(paramName, "PrimaryArmHingeAxisY_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(-gunAimChainAttrib->m_primaryArmHingeAxis.y, paramName);
					sprintf_s(paramName, "PrimaryArmHingeAxisZ_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(-gunAimChainAttrib->m_primaryArmHingeAxis.z, paramName);
				}
				else
				{
					sprintf_s(paramName, "PrimaryArmHingeAxisX_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(gunAimChainAttrib->m_primaryArmHingeAxis.x, paramName);
					sprintf_s(paramName, "PrimaryArmHingeAxisY_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(gunAimChainAttrib->m_primaryArmHingeAxis.y, paramName);
					sprintf_s(paramName, "PrimaryArmHingeAxisZ_%d", animSetIdx + 1);
					nodeDataBlock->writeFloat(gunAimChainAttrib->m_primaryArmHingeAxis.z, paramName);
				}

				sprintf_s(paramName, "UseSecondaryArm_%d", animSetIdx + 1);
				nodeDataBlock->writeBool(gunAimChainAttrib->m_useSecondaryArm, paramName);

				if (gunAimChainAttrib->m_useSecondaryArm)
				{
					sprintf_s(paramName, "SecondaryWristJointIndex_%d", animSetIdx + 1);
					nodeDataBlock->writeUInt(gunAimChainAttrib->m_secondaryWristJointIndex, paramName);

					const bool flipSecondaryHinge = isAxisFlipped(gunAimChainAttrib->m_secondaryArmHingeAxis);

					sprintf_s(paramName, "FlipSecondaryHinge_%d", animSetIdx + 1);
					nodeDataBlock->writeBool(isAxisFlipped(gunAimChainAttrib->m_secondaryArmHingeAxis), paramName);

					if (flipSecondaryHinge)
					{
						sprintf_s(paramName, "SecondaryArmHingeAxisX_%d", animSetIdx + 1);
						nodeDataBlock->writeFloat(-gunAimChainAttrib->m_secondaryArmHingeAxis.x, paramName);
						sprintf_s(paramName, "SecondaryArmHingeAxisY_%d", animSetIdx + 1);
						nodeDataBlock->writeFloat(-gunAimChainAttrib->m_secondaryArmHingeAxis.y, paramName);
						sprintf_s(paramName, "SecondaryArmHingeAxisZ_%d", animSetIdx + 1);
						nodeDataBlock->writeFloat(-gunAimChainAttrib->m_secondaryArmHingeAxis.z, paramName);
					}
					else
					{
						sprintf_s(paramName, "SecondaryArmHingeAxisX_%d", animSetIdx + 1);
						nodeDataBlock->writeFloat(gunAimChainAttrib->m_secondaryArmHingeAxis.x, paramName);
						sprintf_s(paramName, "SecondaryArmHingeAxisY_%d", animSetIdx + 1);
						nodeDataBlock->writeFloat(gunAimChainAttrib->m_secondaryArmHingeAxis.y, paramName);
						sprintf_s(paramName, "SecondaryArmHingeAxisZ_%d", animSetIdx + 1);
						nodeDataBlock->writeFloat(gunAimChainAttrib->m_secondaryArmHingeAxis.z, paramName);
					}
				}

				sprintf_s(paramName, "SpineBias_%d", animSetIdx + 1);
				nodeDataBlock->writeFloat(gunAimChainAttrib->m_spineBias, paramName);

				sprintf_s(paramName, "UpdateTargetByDeltas_%d", animSetIdx + 1);
				nodeDataBlock->writeBool(gunAimChainAttrib->m_updateTargetByDeltas, paramName);

				sprintf_s(paramName, "SpineRootJointIndex_%d", animSetIdx + 1);
				nodeDataBlock->writeUInt(gunAimChainAttrib->m_spineRootJointIndex, paramName);
			}

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeExportTwoBoneIK::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_TWO_BONE_IK);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "EffectorTarget", nodeDef->getInputCPConnection(0), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "TargetOrientation", nodeDef->getInputCPConnection(1), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "SwivelAngle", nodeDef->getInputCPConnection(2), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "IkFkBlendWeight", nodeDef->getInputCPConnection(3), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "SwivelContributionToOrientation", nodeDef->getInputCPConnection(4), true);

			const MR::AttribDataTwoBoneIKSetup* twoBoneIKSetup = static_cast<MR::AttribDataTwoBoneIKSetup*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			//Sanity check because I'm not sure the struct definition for this attrib data is correct since the game uses a different version of it than the morpheme SDK
			if ((twoBoneIKSetup->m_userControlledOrientation && 
				(twoBoneIKSetup->m_useSpecifiedJointOrientation || (nodeDef->getInputCPConnectionSourceNodeID(1) != MR::INVALID_NODE_ID))))
				g_appLog->panicMessage("Invalid TwoBoneIK setup attribute for node %d", nodeDef->getNodeID());

			nodeDataBlock->writeBool(twoBoneIKSetup->m_assumeSimpleHierarchy, "AssumeSimpleHierarchy");
			nodeDataBlock->writeBool(twoBoneIKSetup->m_keepEndEffOrientation, "KeepEndEffOrientation");
			nodeDataBlock->writeBool(twoBoneIKSetup->m_updateTargetByDeltas, "UpdateTargetByDeltas");
			nodeDataBlock->writeBool(twoBoneIKSetup->m_useSpecifiedJointAsTarget, "UseSpecifiedJointAsTarget");
			nodeDataBlock->writeBool(twoBoneIKSetup->m_useSpecifiedJointOrientation, "UseSpecifiedJointOrientation");

			CHAR paramName[256];
			for (uint32_t animSetIndex = 0; animSetIndex < numAnimSets; animSetIndex++)
			{
				const MR::AttribDataTwoBoneIKChain* twoBoneIKChain = static_cast<MR::AttribDataTwoBoneIKChain*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET));
			
				const bool flipMidJointRotationDirection = isAxisFlipped(twoBoneIKChain->m_midJointRotAxis);

				sprintf_s(paramName, "FlipMidJointRotationDirection_%d", animSetIndex + 1);
				nodeDataBlock->writeBool(flipMidJointRotationDirection, paramName);

				if (flipMidJointRotationDirection)
				{
					sprintf_s(paramName, "MidJointRotationAxisX_%d", animSetIndex + 1);
					nodeDataBlock->writeFloat(-twoBoneIKChain->m_midJointRotAxis.x, paramName);
					sprintf_s(paramName, "MidJointRotationAxisY_%d", animSetIndex + 1);
					nodeDataBlock->writeFloat(-twoBoneIKChain->m_midJointRotAxis.y, paramName);
					sprintf_s(paramName, "MidJointRotationAxisZ_%d", animSetIndex + 1);
					nodeDataBlock->writeFloat(-twoBoneIKChain->m_midJointRotAxis.z, paramName);
				}
				else
				{
					sprintf_s(paramName, "MidJointRotationAxisX_%d", animSetIndex + 1);
					nodeDataBlock->writeFloat(twoBoneIKChain->m_midJointRotAxis.x, paramName);
					sprintf_s(paramName, "MidJointRotationAxisY_%d", animSetIndex + 1);
					nodeDataBlock->writeFloat(twoBoneIKChain->m_midJointRotAxis.y, paramName);
					sprintf_s(paramName, "MidJointRotationAxisZ_%d", animSetIndex + 1);
					nodeDataBlock->writeFloat(twoBoneIKChain->m_midJointRotAxis.z, paramName);
				}

				sprintf_s(paramName, "MidJointReferenceAxisX_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(twoBoneIKChain->m_midJointReferenceAxis.x, paramName);
				sprintf_s(paramName, "MidJointReferenceAxisX_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(twoBoneIKChain->m_midJointReferenceAxis.y, paramName);
				sprintf_s(paramName, "MidJointReferenceAxisX_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(twoBoneIKChain->m_midJointReferenceAxis.z, paramName);

				sprintf_s(paramName, "EndJointIndex_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(twoBoneIKChain->m_endJointIndex, paramName);

				sprintf_s(paramName, "MidJointIndex_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(twoBoneIKChain->m_midJointIndex, paramName);

				sprintf_s(paramName, "RootJointIndex_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(twoBoneIKChain->m_rootJointIndex, paramName);

				if (twoBoneIKSetup->m_useSpecifiedJointAsTarget)
				{
					sprintf_s(paramName, "TargetJointIndex_%d", animSetIndex + 1);
					nodeDataBlock->writeUInt(twoBoneIKChain->m_targetJointIndex, paramName);
				}

				sprintf_s(paramName, "GlobalReferenceAxis_%d", animSetIndex + 1);
				nodeDataBlock->writeBool(twoBoneIKChain->m_globalReferenceAxis, paramName);
			}

			return nodeExportXML;
		}

		void NodeExportBasicUnevenTerrain::writeChainAttrib(ME::DataBlockExportXML* nodeDataBlock, uint32_t animSetIdx, MR::AttribDataBasicUnevenTerrainChain* chainAttrib)
		{
			CHAR paramName[256];

			sprintf_s(paramName, "HipsIndex_%d", animSetIdx + 1);
			nodeDataBlock->writeUInt(chainAttrib->m_hipsChannelID, paramName);

			sprintf_s(paramName, "HipsHeightControlEnable_%d", animSetIdx + 1);
			nodeDataBlock->writeBool(chainAttrib->m_hipsHeightControlEnable, paramName);

			sprintf_s(paramName, "HipsPosVelLimitEnable_%d", animSetIdx + 1);
			nodeDataBlock->writeBool(chainAttrib->m_hipsPosVelLimitEnable, paramName);

			sprintf_s(paramName, "HipsPosVelLimit_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(chainAttrib->m_hipsPosVelLimit, paramName);

			sprintf_s(paramName, "HipsPosAccelLimitEnable_%d", animSetIdx + 1);
			nodeDataBlock->writeBool(chainAttrib->m_hipsPosAccelLimitEnable, paramName);

			sprintf_s(paramName, "HipsPosAccelLimit_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(chainAttrib->m_hipsPosAccelLimit, paramName);

			// Common leg options
			sprintf_s(paramName, "StraightestLegFactor_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(chainAttrib->m_straightestLegFactor, paramName);

			sprintf_s(paramName, "AnklePosVelLimitEnable_%d", animSetIdx + 1);
			nodeDataBlock->writeBool(chainAttrib->m_endJointPosVelLimitEnable, paramName);

			sprintf_s(paramName, "AnklePosVelLimit_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(chainAttrib->m_endJointPosVelLimit, paramName);

			sprintf_s(paramName, "AnklePosAccelLimitEnable_%d", animSetIdx + 1);
			nodeDataBlock->writeBool(chainAttrib->m_endJointPosAccelLimitEnable, paramName);

			sprintf_s(paramName, "AnklePosAccelLimit_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(chainAttrib->m_endJointPosAccelLimit, paramName);

			sprintf_s(paramName, "AnkleAngVelLimitEnable_%d", animSetIdx + 1);
			nodeDataBlock->writeBool(chainAttrib->m_endJointAngVelLimitEnable, paramName);

			sprintf_s(paramName, "AnkleAngVelLimit_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(chainAttrib->m_endJointAngVelLimit / (2 * NM_PI), paramName);

			sprintf_s(paramName, "AnkleAngAccelLimitEnable_%d", animSetIdx + 1);
			nodeDataBlock->writeBool(chainAttrib->m_endJointAngAccelLimitEnable, paramName);

			sprintf_s(paramName, "AnkleAngAccelLimit_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(chainAttrib->m_endJointAngAccelLimit / (2 * NM_PI), paramName);

			sprintf_s(paramName, "UseGroundPenetrationFixup_%d", animSetIdx + 1);
			nodeDataBlock->writeBool(chainAttrib->m_useGroundPenetrationFixup, paramName);

			sprintf_s(paramName, "UseTrajectorySlopeAlignment_%d", animSetIdx + 1);
			nodeDataBlock->writeBool(chainAttrib->m_useTrajectorySlopeAlignment, paramName);

			sprintf_s(paramName, "FootAlignToSurfaceAngleLimit_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(getAngleFromClampedCosine(chainAttrib->m_footAlignToSurfaceAngleLimit), paramName);

			sprintf_s(paramName, "FootAlignToSurfaceMaxSlopeAngle_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(getAngleFromClampedCosine(chainAttrib->m_footAlignToSurfaceMaxSlopeAngle), paramName);

			sprintf_s(paramName, "FootLiftingHeightLimit_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(chainAttrib->m_footLiftingHeightLimit, paramName);

			//Leg IK Chain options
			assert(chainAttrib->m_numLimbs == 2);

			const CHAR* legNames[2] = { "Left", "Right" };

			for (uint32_t i = 0; i < 2; i++)
			{
				MR::AttribDataBasicUnevenTerrainChain::ChainData* chainData = chainAttrib->m_chainInfo[i];

				sprintf_s(paramName, "%sHipIndex_%d", legNames[i], animSetIdx + 1);
				nodeDataBlock->writeUInt(chainData->m_channelIDs[0], paramName);

				sprintf_s(paramName, "%sKneeIndex_%d", legNames[i], animSetIdx + 1);
				nodeDataBlock->writeUInt(chainData->m_channelIDs[1], paramName);

				sprintf_s(paramName, "%sAnkleIndex_%d", legNames[i], animSetIdx + 1);
				nodeDataBlock->writeUInt(chainData->m_channelIDs[2], paramName);

				sprintf_s(paramName, "%sBallIndex_%d", legNames[i], animSetIdx + 1);
				nodeDataBlock->writeUInt(chainData->m_channelIDs[3], paramName);

				sprintf_s(paramName, "%sToeIndex_%d", legNames[i], animSetIdx + 1);
				nodeDataBlock->writeUInt(chainData->m_channelIDs[4], paramName);

				const bool flippedAxis = isAxisFlipped(chainData->m_kneeRotationAxis);

				sprintf_s(paramName, "%sFlipKneeRotationDirection_%d", legNames[i], animSetIdx + 1);
				nodeDataBlock->writeBool(flippedAxis, paramName);

				if (flippedAxis)
				{
					sprintf_s(paramName, "%sKneeRotationAxisX_%d", legNames[i], animSetIdx + 1);
					nodeDataBlock->writeFloat(-chainData->m_kneeRotationAxis.x, paramName);

					sprintf_s(paramName, "%sKneeRotationAxisY_%d", legNames[i], animSetIdx + 1);
					nodeDataBlock->writeFloat(-chainData->m_kneeRotationAxis.y, paramName);

					sprintf_s(paramName, "%sKneeRotationAxisZ_%d", legNames[i], animSetIdx + 1);
					nodeDataBlock->writeFloat(-chainData->m_kneeRotationAxis.z, paramName);
				}
				else
				{
					sprintf_s(paramName, "%sKneeRotationAxisX_%d", legNames[i], animSetIdx + 1);
					nodeDataBlock->writeFloat(chainData->m_kneeRotationAxis.x, paramName);

					sprintf_s(paramName, "%sKneeRotationAxisY_%d", legNames[i], animSetIdx + 1);
					nodeDataBlock->writeFloat(chainData->m_kneeRotationAxis.y, paramName);

					sprintf_s(paramName, "%sKneeRotationAxisZ_%d", legNames[i], animSetIdx + 1);
					nodeDataBlock->writeFloat(chainData->m_kneeRotationAxis.z, paramName);
				}
			}
		}

		ME::NodeExportXML* NodeExportBasicUnevenTerrain::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_BASIC_UNEVEN_TERRAIN);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "IkHipsWeight", nodeDef->getInputCPConnection(0), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "IkFkBlendWeight", nodeDef->getInputCPConnection(1), true);

			MR::AttribDataBasicUnevenTerrainSetup* setupAttrib = static_cast<MR::AttribDataBasicUnevenTerrainSetup*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			uint32_t upAxisIndex = getUpAxisIndex(setupAttrib->m_upAxis);
			nodeDataBlock->writeUInt(upAxisIndex, "UpAxisIndex");

			CHAR paramName[256];
			for (uint32_t animSetIdx = 0; animSetIdx < numAnimSets; animSetIdx++)
			{
				MR::AttribDataBasicUnevenTerrainChain* chainAttrib = static_cast<MR::AttribDataBasicUnevenTerrainChain*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, animSetIdx));

				writeChainAttrib(nodeDataBlock, animSetIdx, chainAttrib);
			}

			return nodeExportXML;
		}

		void NodeExportPredictiveUnevenTerrain::writeChainAttrib(ME::DataBlockExportXML* nodeDataBlock, uint32_t animSetIdx, MR::AttribDataPredictiveUnevenTerrainPredictionDef* predictionDefAttrib)
		{
			CHAR paramName[256];

			sprintf_s(paramName, "PredictionSlopeAngleLimit_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(NMP::radiansToDegrees(atanf(predictionDefAttrib->m_footLiftingSlopeAngleLimit)), paramName);

			sprintf_s(paramName, "PredictionLateralAngleLimit_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(NMP::radiansToDegrees(atanf(predictionDefAttrib->m_footLiftingLateralAngleLimit)), paramName);

			sprintf_s(paramName, "PredictionCloseFootbaseTolFrac_%d", animSetIdx + 1);
			nodeDataBlock->writeFloat(predictionDefAttrib->m_closeFootbaseTolFrac, paramName);

			assert(predictionDefAttrib->m_numLimbs == 2);

			// Limb data
			const CHAR* legNames[2] = { "Left", "Right" };
			for (uint32_t i = 0; i < 2; i++)
			{
				MR::AttribDataPredictiveUnevenTerrainPredictionDef::LimbData* limbData = predictionDefAttrib->m_limbInfo[i];

				sprintf_s(paramName, "%sEventTrackID_%d", legNames[i], animSetIdx + 1);
				nodeDataBlock->writeUInt(limbData->m_eventTrackID, paramName);
			}
		}

		ME::NodeExportXML* NodeExportPredictiveUnevenTerrain::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "IkHipsWeight", nodeDef->getInputCPConnection(0), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "IkFkBlendWeight", nodeDef->getInputCPConnection(1), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "PredictionEnable", nodeDef->getInputCPConnection(2), true);

			MR::AttribDataBasicUnevenTerrainSetup* setupAttrib = static_cast<MR::AttribDataBasicUnevenTerrainSetup*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));
			
			uint32_t upAxisIndex = getUpAxisIndex(setupAttrib->m_upAxis);
			nodeDataBlock->writeUInt(upAxisIndex, "UpAxisIndex");

			CHAR paramName[256];
			for (uint32_t animSetIdx = 0; animSetIdx < numAnimSets; animSetIdx++)
			{
				MR::AttribDataBasicUnevenTerrainChain* chainAttrib = static_cast<MR::AttribDataBasicUnevenTerrainChain*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, animSetIdx));

				NodeExportBasicUnevenTerrain::writeChainAttrib(nodeDataBlock, animSetIdx, chainAttrib);

				MR::AttribDataPredictiveUnevenTerrainPredictionDef* predictionDefAttrib = static_cast<MR::AttribDataPredictiveUnevenTerrainPredictionDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_DEF, animSetIdx));
			
				writeChainAttrib(nodeDataBlock, animSetIdx, predictionDefAttrib);
			}

			return nodeExportXML;
		}
	}
}