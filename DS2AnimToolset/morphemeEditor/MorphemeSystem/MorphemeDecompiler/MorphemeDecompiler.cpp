#include "MorphemeDecompiler.h"
#include "RCore.h"
#include "extern.h"
#include "MorphemeSystem/MorphemeUtils/MorphemeUtils.h"
#include "morpheme/mrCharacterControllerDef.h"
#include "morpheme/mrMirroredAnimMapping.h"
#include "NMGeomUtils/NMJointLimits.h"

#include "Node/Node.h"
#include "Node/StateMachineNode.h"
#include "Node/ControlParamNode.h"
#include "Node/BlendNode.h"
#include "Node/TransitNode.h"
#include "Node/IKNode.h"
#include "Node/OperatorNode.h"

namespace MD
{
	ME::TakeListXML* exportAnimMarkup(MorphemeCharacterDef* character, int animId, std::wstring dstFileName, int fps)
	{
		MR::NetworkDef* netDef = character->getNetworkDef();

		MR::NodeDef* animNode = MorphemeUtils::getAnimNodeByAnimID(netDef, animId);

		ME::ExportFactoryXML factory;
		ME::TakeListXML* takeList = (ME::TakeListXML*)factory.createTakeList(dstFileName.c_str(), dstFileName.c_str());

		if (animNode == nullptr)
		{
			takeList->createTake(RString::toWide(character->getAnimFileLookUp()->getTakeName(animId)).c_str(), 1, fps, true);
			return takeList;
		}

		MR::AttribDataBool* loopAttr = static_cast<MR::AttribDataBool*>(animNode->getAttribData(MR::ATTRIB_SEMANTIC_LOOP));
		MR::AttribDataSourceAnim* sourceAnim = static_cast<MR::AttribDataSourceAnim*>(animNode->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM));
		MR::AttribDataSourceEventTrackSet* sourceEvents = static_cast<MR::AttribDataSourceEventTrackSet*>(animNode->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS));

		bool loop = false;

		if (loopAttr != nullptr)
			loop = loopAttr->m_value;

		ME::TakeExportXML* take = static_cast<ME::TakeExportXML*>(takeList->createTake(RString::toWide(character->getAnimFileLookUp()->getTakeName(animId)).c_str(), sourceAnim->m_sourceAnimDuration, fps, loop, sourceAnim->m_clipStartFraction, sourceAnim->m_clipEndFraction));

		for (size_t i = 0; i < sourceEvents->m_numDiscreteEventTracks; i++)
		{
			MR::EventTrackDefDiscrete* track = sourceEvents->m_sourceDiscreteEventTracks[i];

			GUID gidReference;
			CoCreateGuid(&gidReference);

			std::string guid = RString::guidToString(gidReference);

			ME::DiscreteEventTrackExportXML* trackXML = (ME::DiscreteEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE, guid.c_str(), RString::toWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

			for (size_t j = 0; j < track->getNumEvents(); j++)
				trackXML->createEvent(j, track->getEvent(j)->getStartTime(), track->getEvent(j)->getUserData());
		}

		for (size_t i = 0; i < sourceEvents->m_numCurveEventTracks; i++)
		{
			MR::EventTrackDefCurve* track = sourceEvents->m_sourceCurveEventTracks[i];

			GUID gidReference;
			CoCreateGuid(&gidReference);

			std::string guid = RString::guidToString(gidReference);

			ME::CurveEventTrackExportXML* trackXML = (ME::CurveEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_CURVE, guid.c_str(), RString::toWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

			for (size_t j = 0; j < track->getNumEvents(); j++)
				trackXML->createEvent(j, track->getEvent(j)->getTime(), track->getEvent(j)->getValue(), track->getEvent(j)->getUserData());
		}

		for (size_t i = 0; i < sourceEvents->m_numDurEventTracks; i++)
		{
			MR::EventTrackDefDuration* track = sourceEvents->m_sourceDurEventTracks[i];

			GUID gidReference;
			CoCreateGuid(&gidReference);

			std::string guid = RString::guidToString(gidReference);

			ME::DurationEventTrackExportXML* trackXML = (ME::DurationEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION, guid.c_str(), RString::toWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

			for (size_t j = 0; j < track->getNumEvents(); j++)
				trackXML->createEvent(j, track->getEvent(j)->getStartTime(), track->getEvent(j)->getDuration(), track->getEvent(j)->getUserData());
		}

		return takeList;
	}

	ME::CharacterControllerExportXML* exportCharacterController(MR::CharacterControllerDef* characterController, std::wstring dstFileName)
	{
		ME::ExportFactoryXML factory;

		GUID gidReference;
		CoCreateGuid(&gidReference);

		ME::CharacterControllerExportXML* characterControllerExport = static_cast<ME::CharacterControllerExportXML*>(factory.createCharacterController(RString::guidToString(gidReference).c_str(), dstFileName.c_str(), "Controller"));

		ME::AttributeBlockExport* attributes = characterControllerExport->getAttributeBlock();

		if (attributes)
		{
			MR::CharacterControllerDef::ShapeType shapeType = characterController->getShape();

			switch (shapeType)
			{
			case MR::CharacterControllerDef::BoxShape:
				attributes->createStringAttribute("Shape", "Box");
				attributes->createDoubleAttribute("Height", characterController->getHeight());
				attributes->createDoubleAttribute("Width", characterController->getWidth());
				attributes->createDoubleAttribute("Depth", characterController->getDepth());
				break;
			case MR::CharacterControllerDef::CapsuleShape:
				attributes->createStringAttribute("Shape", "Capsule");
				attributes->createDoubleAttribute("Height", characterController->getHeight());
				attributes->createDoubleAttribute("Radius", characterController->getRadius());
				break;
			case MR::CharacterControllerDef::CylinderShape:
				attributes->createStringAttribute("Shape", "Cylinder");
				attributes->createDoubleAttribute("Height", characterController->getHeight());
				attributes->createDoubleAttribute("Radius", characterController->getRadius());
				break;
			case MR::CharacterControllerDef::SphereShape:
				attributes->createStringAttribute("Shape", "Sphere");
				attributes->createDoubleAttribute("Radius", characterController->getRadius());
				break;
			default:
				break;
			}

			attributes->createVector3Attribute("Colour", NMP::Vector3(characterController->getColour().getRf(), characterController->getColour().getGf(), characterController->getColour().getBf()));
			attributes->createBoolAttribute("Visible", characterController->getVisible());
			attributes->createDoubleAttribute("SkinWidth", characterController->getSkinWidth());
			attributes->createDoubleAttribute("MaxPushForce", characterController->getMaxPushForce());
			attributes->createDoubleAttribute("MaxSlopeAngle", RMath::degToRad(characterController->getMaxSlopeAngle()));
			attributes->createDoubleAttribute("StepHeight", characterController->getStepHeight());
		}

		return characterControllerExport;
	}

	ME::RigExportXML* exportRig(MR::NetworkDef* netDef, MR::AnimRigDef* rig, int animSetIdx, std::wstring dstFileName)
	{
		ME::ExportFactoryXML factory;

		GUID gidReference;
		CoCreateGuid(&gidReference);

		ME::RigExportXML* rigExport = static_cast<ME::RigExportXML*>(factory.createRig(RString::guidToString(gidReference).c_str(), dstFileName.c_str(), RString::removeExtension(RString::toNarrow(dstFileName.c_str())).c_str()));

		for (size_t i = 0; i < rig->getNumBones(); i++)
		{
			ME::JointExportXML* jointExport = static_cast<ME::JointExportXML*>(rigExport->createJoint(i, RString::toWide(rig->getBoneName(i)).c_str(), rig->getParentBoneIndex(i)));

			jointExport->setBodyPlanTag(rig->getBoneName(i));
			jointExport->setRetargetingTag(rig->getBoneName(i));

			const NMP::Vector3* translation = rig->getBindPoseBonePos(i);
			jointExport->setTranslation(translation->x, translation->y, translation->z);

			const NMP::Quat* rotation = rig->getBindPoseBoneQuat(i);
			jointExport->setRotation(rotation->x, rotation->y, rotation->z, rotation->w);
		}

		const MR::AttribDataMirroredAnimMapping* mirroredMappingAttrib = static_cast<MR::AttribDataMirroredAnimMapping*>(netDef->getNodeDef(0)->getAttribData(MR::ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING, animSetIdx));

		if (mirroredMappingAttrib != nullptr)
		{
			for (size_t i = 0; i < mirroredMappingAttrib->getNumMappings(); i++)
				rigExport->createMirrorMapping(i, mirroredMappingAttrib->getLeftBone(i), mirroredMappingAttrib->getRightBone(i));
		}

		const MR::AttribDataJointLimits* jointLimitAttrib = static_cast<MR::AttribDataJointLimits*>(netDef->getNodeDef(0)->getAttribData(MR::ATTRIB_SEMANTIC_JOINT_LIMITS, animSetIdx));

		if (jointLimitAttrib != nullptr)
		{
			for (size_t i = 0; i < jointLimitAttrib->getNumRigJoints(); i++)
			{
				const NMRU::JointLimits::Params* jointLimitParams = jointLimitAttrib->getLimitForJoint(i);

				if (jointLimitParams != nullptr)
				{
					ME::JointLimitExportXML* jointLimitExport = static_cast<ME::JointLimitExportXML*>(rigExport->createJointLimit(rigExport->getNumJointLimits()));

					jointLimitExport->setJointIndex(i);
					jointLimitExport->setOrientation(jointLimitParams->frame.x, jointLimitParams->frame.y, jointLimitParams->frame.z, jointLimitParams->frame.w);
					jointLimitExport->setOffsetOrientation(jointLimitParams->offset.x, jointLimitParams->offset.y, jointLimitParams->offset.z, jointLimitParams->offset.w);

					float twistMin = atanf(jointLimitParams->lower.x) * 4.f;
					float twistMax = atanf(jointLimitParams->upper.x) * 4.f;
					float lean1 = atanf(jointLimitParams->upper.y) * 4.f;
					float lean2 = atanf(jointLimitParams->upper.z) * 4.f;

					jointLimitExport->setTwistLow(-twistMax);
					jointLimitExport->setTwistHigh(-twistMin);

					ME::JointLimitExport::LimitType limitType = ME::JointLimitExport::kHingeLimitType;

					if (lean1 && lean2)
					{
						jointLimitExport->setSwing1(lean2);
						jointLimitExport->setSwing2(lean1);

						limitType = ME::JointLimitExport::kBallSocketLimitType;
					}

					jointLimitExport->setLimitType(limitType);
				}
			}
		}

		rigExport->setHipIndex(rig->getCharacterRootBoneIndex());
		rigExport->setTrajectoryIndex(rig->getTrajectoryBoneIndex());

		const NMP::Vector3* blendFrameTranslation = rig->getBlendFrameTranslation();
		rigExport->setBlendFramePositionVec(blendFrameTranslation->x, blendFrameTranslation->y, blendFrameTranslation->z);

		const NMP::Quat* blendFrameOrientation = rig->getBlendFrameOrientation();
		rigExport->setBlendFrameOrientationQuat(blendFrameOrientation->x, blendFrameOrientation->y, blendFrameOrientation->z, blendFrameOrientation->w);

		rigExport->setRigRetargetScale(1.f);
		rigExport->setMirrorPlane(0);
		
		return rigExport;
	}

	ME::AnimationLibraryXML* exportAnimLibrary(MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* filenameLookup, MR::NetworkDef* netDef, std::vector<ME::RigExportXML*> rigExport, std::vector<ME::CharacterControllerExportXML*> controllerExport, std::wstring chrName, std::wstring dstFileName, const char* animFormat, bool resample, int sampleFreq)
	{
		ME::ExportFactoryXML factory;

		GUID gidReference;
		CoCreateGuid(&gidReference);

		ME::AnimationLibraryXML* animLibraryExport = static_cast<ME::AnimationLibraryXML*>(factory.createAnimationLibrary(RString::guidToString(gidReference).c_str(), L"ConnectLibrary", dstFileName.c_str()));

		for (size_t i = 0; i < netDef->getNumAnimSets(); i++)
		{
			wchar_t setName[260];
			swprintf_s(setName, L"%ws_%d", chrName.c_str(), i);

			ME::AnimationSetExportXML* animSetExport = static_cast<ME::AnimationSetExportXML*>(animLibraryExport->createAnimationSet(i, setName));
		
			if (controllerExport[i])
				animSetExport->setCharacterController(controllerExport[i]);

			if (rigExport[i])
				animSetExport->setRig(rigExport[i]);

			MR::AttribDataMirroredAnimMapping* mirroredMapping = static_cast<MR::AttribDataMirroredAnimMapping*>(netDef->getNodeDef(0)->getAttribData(MR::ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING));

			if (mirroredMapping)
			{
				for (size_t i = 0; i < mirroredMapping->getNumEventIdsToRemap(); i++)
					animSetExport->createEventUserdataMirrorMapping(i, mirroredMapping->getLeftEventIdMapping(i), mirroredMapping->getRightEventIdMapping(i));

				for (size_t i = 0; i < mirroredMapping->getNumTrackIdsToRemap(); i++)
					animSetExport->createEventTrackMirrorMapping(i, mirroredMapping->getLeftTrackIdMapping(i), mirroredMapping->getRightTrackIdMapping(i));
			}

			for (size_t i = 0; i < filenameLookup->getNumAnims(); i++)
			{
				const char* sourceFileName = filenameLookup->getSourceFilename(i);

				wchar_t takeFile[260];
				swprintf_s(takeFile, L"$(RootDir)\\morphemeMarkup\\%s.xml", std::filesystem::path(sourceFileName).filename().c_str());

				std::string options = "";

				if (resample)
					options = "-resample " + std::to_string(sampleFreq);

				animSetExport->createAnimationEntry(i, RString::toWide(sourceFileName).c_str(), takeFile, RString::toWide(filenameLookup->getTakeName(i)).c_str(), L"Footsteps", animFormat, options.c_str());
			}
		}

		return animLibraryExport;
	}

	ME::MessagePresetLibraryExportXML* exportMessagePresetLibrary(MR::NetworkDef* netDef, std::wstring chrName, std::wstring dstFileName)
	{
		ME::ExportFactoryXML factory;

		GUID gidReference;
		CoCreateGuid(&gidReference);

		ME::MessagePresetLibraryExportXML* messagePresetLibraryExport = static_cast<ME::MessagePresetLibraryExportXML*>(factory.createMessagePresetLibrary(RString::guidToString(gidReference).c_str(), dstFileName.c_str()));

		return messagePresetLibraryExport;
	}

	ME::NetworkDefExportXML* exportNetwork(MR::NetworkDef* netDef, ME::AnimationLibraryXML* animLibraryExport, ME::MessagePresetLibraryExportXML* messagePresetLibraryExport, std::wstring chrName, std::wstring dstFileName)
	{
		ME::ExportFactoryXML factory;

		GUID gidReference;
		CoCreateGuid(&gidReference);

		wchar_t netName[260];
		swprintf_s(netName, L"%ws", chrName.c_str());

		ME::NetworkDefExportXML* netDefExport = static_cast<ME::NetworkDefExportXML*>(factory.createNetworkDef(RString::guidToString(gidReference).c_str(), netName, dstFileName.c_str()));

		netDefExport->setNetworkWorldOrientation(NMP::Vector3YAxis(), NMP::Vector3XAxis(), NMP::Vector3ZAxis());

		for (size_t i = 0; i < netDef->getNumNodeDefs(); i++)
			exportNode(netDefExport, netDef, i, netDef->getNodeNameFromNodeID(i));

		const NMP::IDMappedStringTable* messageTable = netDef->getMessageIDNamesTable();

		for (size_t i = 0; i < netDef->getNumMessages(); i++)
		{
			int messageId = netDef->getMessageDistributor(i)->m_messageID;
			int messageType = netDef->getMessageDistributor(i)->m_messageType;
			const char* messageName = netDef->getMessageNameFromMessageID(messageId);

			g_appLog->debugMessage(MsgLevel_Info, "\tExporting message %d (name=\"%s\", typeId=%d)\n", messageId, messageName, messageType);

			netDefExport->createMessage(RString::toWide(messageName).c_str(), messageType, messageId);
		}

		netDefExport->setRootNodeNetworkID(netDef->getRootNodeID());

		if (animLibraryExport)
			netDefExport->setAnimationLibrary(animLibraryExport);

		if (messagePresetLibraryExport)
			netDefExport->setMessagePresetLibrary(messagePresetLibraryExport);

		return netDefExport;
	}

	ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, int nodeId, std::string nodeName)
	{
		Node::NodeDecompilerBase* nodeExport = nullptr;
		MR::NodeDef* nodeDef = netDef->getNodeDef(nodeId);
		MR::NodeType nodeTypeID = nodeDef->getNodeTypeID();

		g_appLog->debugMessage(MsgLevel_Info, "\tExporting node %d (name=\"%s\", typeId=%d)\n", nodeId, nodeName.c_str(), nodeTypeID);

		switch (nodeTypeID)
		{
		case NODE_TYPE_NETWORK:
			nodeExport = new Node::NodeNetworkDecompiler;
			break;
		case NODE_TYPE_STATE_MACHINE:
			nodeExport = new Node::NodeStateMachineDecompiler;
			break;
		case NODE_TYPE_CP_FLOAT:
			nodeExport = new Node::NodeCPFloatDecompiler;
			break;
		case NODE_TYPE_CP_VECTOR3:
			nodeExport = new Node::NodeCPVector3Decompiler;
			break;
		case NODE_TYPE_CP_VECTOR4:
			nodeExport = new Node::NodeCPVector4Decompiler;
			break;
		case NODE_TYPE_CP_BOOL:
			nodeExport = new Node::NodeCPBoolDecompiler;
			break;
		case NODE_TYPE_CP_INT:
			nodeExport = new Node::NodeCPIntDecompiler;
			break;
		case NODE_TYPE_CP_UINT:
			nodeExport = new Node::NodeCPUIntDecompiler;
			break;
		case NODE_TYPE_ANIM_EVENTS:
			nodeExport = new Node::NodeAnimSyncEventsDecompiler;
			break;
		case NODE_TYPE_FILTER_TRANSFORMS:
			nodeExport = new Node::NodeFilterTransformsDecompiler;
			break;
		case NODE_TYPE_BLEND_2:
			nodeExport = new Node::NodeBlend2Decompiler;
			break;
		case NODE_TYPE_BLEND_N:
			nodeExport = new Node::NodeBlendNDecompiler;
			break;
		case NODE_TYPE_BLEND_2X2:
			nodeExport = new Node::NodeBlend2x2Decompiler;
			break;
		case NODE_TYPE_BLEND_NXM:
			nodeExport = new Node::NodeBlendNxMDecompiler;
			break;
		case NODE_TYPE_BLEND_ALL:
			nodeExport = new Node::NodeBlendAllDecompiler;
			break;
		case NODE_TYPE_FEATHER_BLEND_2:
			nodeExport = new Node::NodeFeatherBlendDecompiler;
			break;
		case NODE_TYPE_SUBTRACTIVE_BLEND:
			nodeExport = new Node::NodeSubtractiveBlendDecompiler;
			break;
		case NODE_TYPE_SINGLEFRAME:
			nodeExport = new Node::NodeSingleFrameDecompiler;
			break;
		case NODE_TYPE_FREEZE:
			nodeExport = new Node::NodeFreezeDecompiler;
			break;
		case NODE_TYPE_CP_OP_FUNCTION:
			nodeExport = new Node::NodeOperatorFunctionDecompiler;
			break;
		case NODE_TYPE_CP_OP_ARITHMETIC:
			nodeExport = new Node::NodeOperatorArithmeticDecompiler;
			break;
		case NODE_TYPE_CP_OP_ARITHMETIC_VECTOR3:
			nodeExport = new Node::NodeOperatorArithmeticVector3Decompiler;
			break;
		case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC:
			nodeExport = new Node::NodeOperatorOneInputArithmeticDecompiler;
			break;
		case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3:
			nodeExport = new Node::NodeOperatorOneInputArithmeticVector3;
			break;
		case NODE_TYPE_CP_OP_NOISE_GEN:
			nodeExport = new Node::NodeOperatorNoiseGenDecompiler;
			break;
		case NODE_TYPE_CP_OP_INT_TO_FLOAT:
			nodeExport = new Node::NodeOperatorIntToFloatDecompiler;
			break;
		case NODE_TYPE_CP_OP_RAY_CAST:
			nodeExport = new Node::NodeOperatorRayCastDecompiler;
			break;
		case NODE_TYPE_CP_OP_SMOOTH_FLOAT:
			nodeExport = new Node::NodeOperatorSmoothFloatDecompiler;
			break;
		case NODE_TYPE_CP_OP_RAMP_FLOAT:
			nodeExport = new Node::NodeOperatorRampFloatDecompiler;
			break;
		case NODE_TYPE_CP_OP_FLOATS_TO_VECTOR3:
			nodeExport = new Node::NodeOperatorFloatsToVector3Decompiler;
			break;
		case NODE_TYPE_CP_OP_FLOAT_TO_INT:
			nodeExport = new Node::NodeOperatorFloatToIntDecompiler;
			break;
		case NODE_TYPE_CP_OP_RANDOM_FLOAT:
			nodeExport = new Node::NodeOperatorRandomFloatDecompiler;
			break;
		case NODE_TYPE_CP_OP_RATE_OF_CHANGE:
			nodeExport = new Node::NodeOperatorRateOfChangeDecompiler;
			break;
		case NODE_TYPE_CP_OP_VECTOR3_TO_FLOATS:
			nodeExport = new Node::NodeOperatorVector3ToFloatsDecompiler;
			break;
		case NODE_TYPE_CP_OP_VECTOR3_DOT:
			nodeExport = new Node::NodeOperatorVector3DotDecompiler;
			break;
		case NODE_TYPE_CP_OP_VECTOR3_DISTANCE:
			nodeExport = new Node::NodeOperatorVector3DistanceDecompiler;
			break;
		case NODE_TYPE_CP_OP_VECTOR3_CROSSPRODUCT:
			nodeExport = new Node::NodeOperatorVector3CrossProductDecompiler;
			break;
		case NODE_TYPE_CP_OP_VECTOR3_ANGLE:
			nodeExport = new Node::NodeOperatorVector3AngleDecompiler;
			break;
		case NODE_TYPE_CP_OP_VECTOR3_NORMALISE:
			nodeExport = new Node::NodeOperatorVector3NormaliseDecompiler;
			break;
		case NODE_TYPE_APPLY_BIND_POSE:
			nodeExport = new Node::NodeApplyBindPoseDecompiler;
			break;
		case NODE_TYPE_APPLY_GLOBAL_TIME:
			nodeExport = new Node::NodeApplyGlobalTimeDecompiler;
			break;
		case NODE_TYPE_TWO_BONE_IK:
			nodeExport = new Node::NodeTwoBoneIKDecompiler;
			break;
		case NODE_TYPE_LOCK_FOOT:
			nodeExport = new Node::NodeLockFootDecompiler;
			break;
		case NODE_TYPE_HEAD_LOOK:
			nodeExport = new Node::NodeHeadLookDecompiler;
			break;
		case NODE_TYPE_HIPS_IK:
			nodeExport = new Node::NodeHipsIKDecompiler;
			break;
		case NODE_TYPE_PLAY_SPEED_MODIFIER:
			nodeExport = new Node::NodePlaySpeedModifierDecompiler;
			break;
		case NODE_TYPE_SCALE_TO_DURATION:
			nodeExport = new Node::NodeScaleToDurationDecompiler;
			break;
		case NODE_TYPE_EMIT_JOINT_CP_INFO:
			nodeExport = new Node::NodeExtractJointInfoDecompiler;
			break;
		case NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT:
			nodeExport = new Node::NodeEmitRequestOnDiscreteEventDecompiler;
			break;
		case NODE_TYPE_CLOSEST_ANIM:
			nodeExport = new Node::NodeClosestAnimDecompiler;
			break;
		case NODE_TYPE_SWITCH:
			nodeExport = new Node::NodeSwitchDecompiler;
			break;
		case NODE_TYPE_SEQUENCE:
			nodeExport = new Node::NodeSequenceDecompiler;
			break;
		case NODE_TYPE_PASSTHROUGH:
			nodeExport = new Node::NodePassThroughDecompiler;
			break;
		case NODE_MIRROR_TRANSFORMS_ID:
			nodeExport = new Node::NodeMirrorTransformsDecompiler;
			break;
		case NODE_TYPE_BASIC_UNEVEN_TERRAIN:
			nodeExport = new Node::NodeBasicUnevenTerrainDecompiler;
			break;
		case NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN:
			nodeExport = new Node::NodePredictiveUnevenTerrainDecompiler;
			break;
		case NODE_TYPE_RETARGET:
			nodeExport = new Node::NodeRetargetDecompiler;
			break;
		case NODE_TYPE_SCALE_CHARACTER:
			nodeExport = new Node::NodeScaleCharacterDecompiler;
			break;
		case NODE_TYPE_GUN_AIM_IK:
			nodeExport = new Node::NodeGunAimIKDecompiler;
			break;
		case NODE_TYPE_TRANSIT_SYNC_EVENTS:
		case NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS:
			nodeExport = new Node::NodeTransitSyncEventsDecompiler;
			break;
		case NODE_TYPE_TRANSIT:
		case NODE_TYPE_TRANSIT_PHYSICS:
			nodeExport = new Node::NodeTransitDecompiler;
			break;
		case NODE_TYPE_SMOOTH_TRANSFORMS:
			nodeExport = new Node::NodeSmoothTransformsDecompiler;
			break;
		default:
			nodeExport = new Node::NodeUnhandledDecompiler;
			break;
		}

		ME::NodeExportXML* node = nodeExport->exportNode(netDefExport, netDef, nodeDef, nodeName);

		nodeExport->exportNodeTransitions(nodeDef, node);

		delete nodeExport;

		return node;
	}
}