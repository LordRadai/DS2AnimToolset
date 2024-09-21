#include "MorphemeDecompiler.h"
#include "RCore.h"
#include "extern.h"
#include "MorphemeSystem/MRUtils/MRUtils.h"
#include "morpheme/mrCharacterControllerDef.h"
#include "morpheme/mrMirroredAnimMapping.h"

#include "Node/Node.h"
#include "Node/StateMachineNode.h"
#include "Node/ControlParamNode.h"
#include "Node/BlendNode.h"
#include "Node/TransitNode.h"
#include "Node/IKNode.h"

namespace MD
{
	ME::TakeListXML* exportAnimMarkup(MorphemeCharacterDef* character, int animId, std::wstring dstFileName)
	{
		MR::NetworkDef* netDef = character->getNetworkDef();

		MR::NodeDef* animNode = MRUtils::getAnimNodeByAnimID(netDef, animId);

		if (animNode == nullptr)
			return nullptr;

		MR::AttribDataBool* loopAttr = static_cast<MR::AttribDataBool*>(animNode->getAttribData(MR::ATTRIB_SEMANTIC_LOOP));
		MR::AttribDataSourceAnim* sourceAnim = static_cast<MR::AttribDataSourceAnim*>(animNode->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM));
		MR::AttribDataSourceEventTrackSet* sourceEvents = static_cast<MR::AttribDataSourceEventTrackSet*>(animNode->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS));

		bool loop = false;

		if (loopAttr != nullptr)
			loop = loopAttr->m_value;

		int sourceAnimId = sourceAnim->m_animAssetID;

		ME::ExportFactoryXML factory;
		std::wstring dest = dstFileName;

		ME::TakeListXML* takeList = (ME::TakeListXML*)factory.createTakeList(dest.c_str(), dest.c_str());
		ME::TakeExportXML* take = (ME::TakeExportXML*)takeList->createTake(RString::toWide(character->getAnimFileLookUp()->getTakeName(sourceAnimId)).c_str(), sourceAnim->m_sourceAnimDuration, 30, loop, sourceAnim->m_clipStartFraction, sourceAnim->m_clipEndFraction);

		for (size_t i = 0; i < sourceEvents->m_numDiscreteEventTracks; i++)
		{
			GUID gidReference;
			CoCreateGuid(&gidReference);

			MR::EventTrackDefDiscrete* track = sourceEvents->m_sourceDiscreteEventTracks[i];

			ME::DiscreteEventTrackExportXML* trackXML = (ME::DiscreteEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE, RString::guidToString(gidReference).c_str(), RString::toWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

			for (size_t j = 0; j < track->getNumEvents(); j++)
				trackXML->createEvent(j, track->getEvent(j)->getStartTime(), track->getEvent(j)->getUserData());
		}

		for (size_t i = 0; i < sourceEvents->m_numCurveEventTracks; i++)
		{
			GUID gidReference;
			CoCreateGuid(&gidReference);

			MR::EventTrackDefCurve* track = sourceEvents->m_sourceCurveEventTracks[i];

			ME::CurveEventTrackExportXML* trackXML = (ME::CurveEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_CURVE, RString::guidToString(gidReference).c_str(), RString::toWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

			for (size_t j = 0; j < track->getNumEvents(); j++)
				trackXML->createEvent(j, track->getEvent(j)->getTime(), track->getEvent(j)->getValue(), track->getEvent(j)->getUserData());
		}

		for (size_t i = 0; i < sourceEvents->m_numDurEventTracks; i++)
		{
			GUID gidReference;
			CoCreateGuid(&gidReference);

			MR::EventTrackDefDuration* track = sourceEvents->m_sourceDurEventTracks[i];

			ME::DurationEventTrackExportXML* trackXML = (ME::DurationEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION, RString::guidToString(gidReference).c_str(), RString::toWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

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

	ME::RigExportXML* exportRig(MR::NetworkDef* netDef, MR::AnimRigDef* rig, std::wstring dstFileName)
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

		rigExport->setHipIndex(rig->getCharacterRootBoneIndex());
		rigExport->setTrajectoryIndex(rig->getTrajectoryBoneIndex());

		const NMP::Vector3* blendFrameTranslation = rig->getBlendFrameTranslation();
		rigExport->setBlendFramePositionVec(blendFrameTranslation->x, blendFrameTranslation->y, blendFrameTranslation->z);

		const NMP::Quat* blendFrameOrientation = rig->getBlendFrameOrientation();
		rigExport->setBlendFrameOrientationQuat(blendFrameOrientation->x, blendFrameOrientation->y, blendFrameOrientation->z, blendFrameOrientation->w);

		rigExport->setRigRetargetScale(1.f);
		rigExport->setMirrorPlane(0);

		if (netDef->getNodeDef(0)->getNumAttribDataHandles() == 4)
		{
			MR::AttribDataMirroredAnimMapping* mirroredMapping = static_cast<MR::AttribDataMirroredAnimMapping*>(netDef->getNodeDef(0)->getAttribData(MR::ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING));

			for (size_t i = 0; i < mirroredMapping->getNumMappings(); i++)
				rigExport->createJointMapping(i, mirroredMapping->getLeftBone(i), mirroredMapping->getRightBone(i));
		}

		return rigExport;
	}

	ME::AnimationLibraryXML* exportAnimLibrary(MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* filenameLookup, MR::NetworkDef* netDef, std::vector<ME::RigExportXML*> rigExport, std::vector<ME::CharacterControllerExportXML*> controllerExport, std::wstring chrName, std::wstring dstFileName)
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

			if (netDef->getNodeDef(0)->getNumAttribDataHandles() == 4)
			{
				MR::AttribDataMirroredAnimMapping* mirroredMapping = static_cast<MR::AttribDataMirroredAnimMapping*>(netDef->getNodeDef(0)->getAttribData(MR::ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING));

				if (mirroredMapping)
				{
					for (size_t i = 0; i < mirroredMapping->getNumEventIdsToRemap(); i++)
						animSetExport->createEventUserdataMirrorMapping(i, mirroredMapping->getLeftEventIdMapping(i), mirroredMapping->getRightEventIdMapping(i));

					for (size_t i = 0; i < mirroredMapping->getNumTrackIdsToRemap(); i++)
						animSetExport->createEventTrackMirrorMapping(i, mirroredMapping->getLeftTrackIdMapping(i), mirroredMapping->getRightTrackIdMapping(i));
				}
			}

			for (size_t i = 0; i < filenameLookup->getNumAnims(); i++)
			{
				const char* sourceFileName = filenameLookup->getSourceFilename(i);

				wchar_t takeFile[260];
				swprintf_s(takeFile, L"$(RootDir)\\morphemeMarkup\\%s.xml", std::filesystem::path(sourceFileName).filename().c_str());

				animSetExport->createAnimationEntry(i, RString::toWide(sourceFileName).c_str(), takeFile, RString::toWide(filenameLookup->getTakeName(i)).c_str(), L"Footsteps", "nsa", "");
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

		netDefExport->setNetworkWorldOrientation(NMP::Vector3XAxis(), NMP::Vector3ZAxis(), NMP::Vector3XAxis());

		for (size_t i = 0; i < netDef->getNumNodeDefs(); i++)
			exportNode(netDefExport, netDef, i);

		const NMP::IDMappedStringTable* messageTable = netDef->getMessageIDNamesTable();

		for (size_t i = 0; i < netDef->getNumMessages(); i++)
		{
			int messageId = netDef->getMessageDistributor(i)->m_messageID;
			int messageType = netDef->getMessageDistributor(i)->m_messageType;

			netDefExport->createMessage(RString::toWide(netDef->getMessageNameFromMessageID(messageId)).c_str(), messageType, messageId);
		}

		netDefExport->setRootNodeNetworkID(netDef->getRootNodeID());

		if (animLibraryExport)
			netDefExport->setAnimationLibrary(animLibraryExport);

		if (messagePresetLibraryExport)
			netDefExport->setMessagePresetLibrary(messagePresetLibraryExport);

		return netDefExport;
	}

	ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, int nodeId)
	{
		MR::NodeDef* nodeDef = netDef->getNodeDef(nodeId);
		MR::NodeType nodeTypeID = nodeDef->getNodeTypeID();

		g_appLog->debugMessage(MsgLevel_Debug, "Exporting node %d (typeId=%d)\n", nodeId, nodeTypeID);

		ME::NodeExportXML* nodeExport = nullptr;

		switch (nodeTypeID)
		{
		case NODE_TYPE_NETWORK:
			nodeExport = Node::exportNetworkNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_STATE_MACHINE:
			nodeExport = Node::exportStateMachineNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_FLOAT:
			nodeExport = Node::exportCPFloatNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_VECTOR3:
			nodeExport = Node::exportCPVector3Node(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_VECTOR4:
			nodeExport = Node::exportCPVector4Node(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_BOOL:
			nodeExport = Node::exportCPBoolNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_INT:
			nodeExport = Node::exportCPIntNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_UINT:
			nodeExport = Node::exportCPUIntNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_PHYSICS_OBJECT_POINTER:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_ANIMATION:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_ANIM_EVENTS:
			nodeExport = Node::exportAnimSyncEventsNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_FILTER_TRANSFORMS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_BLEND_2:
			nodeExport = Node::exportBlend2Node(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_BLEND_N:
			nodeExport = Node::exportBlendNNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_SINGLEFRAME:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_FREEZE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_BLEND_2X2:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_BLEND_NXM:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_BLEND_ALL:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_SUBTRACTIVE_BLEND:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_SCATTER_BLEND_1D:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_SCATTER_BLEND_2D:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_SCATTER_BLEND_3D:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_BOOLEAN:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_FUNCTION:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_ARITHMETIC:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_ARITHMETIC_VECTOR3:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_NOISE_GEN:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_FEATHER_BLEND_2:
			nodeExport = Node::exportFeatherBlend2Node(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_APPLY_BIND_POSE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_PHYSICS_GROUPER:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_BEHAVIOUR_GROUPER:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_APPLY_GLOBAL_TIME:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_APPLY_PHYSICS_JOINT_LIMITS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_SET_NON_PHYSICS_TRANSFORMS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_TWO_BONE_IK:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_LOCK_FOOT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_HEAD_LOOK:
			nodeExport = Node::exportHeadLookNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_PHYSICS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_BEHAVIOUR:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_HIPS_IK:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_PERFORMANCE_BEHAVIOUR:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_PLAY_SPEED_MODIFIER:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_SCALE_TO_DURATION:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_EMIT_JOINT_CP_INFO:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CLOSEST_ANIM:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_SWITCH:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_SEQUENCE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_PASSTHROUGH:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_MIRROR_TRANSFORMS_ID:
			nodeExport = Node::exportMirrorTransformNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_BASIC_UNEVEN_TERRAIN:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_ACTIVE_STATE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_INT_TO_FLOAT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_RAY_CAST:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_SMOOTH_FLOAT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_RAMP_FLOAT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_FLOATS_TO_VECTOR3:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_FLOAT_TO_INT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_RANDOM_FLOAT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_PHYSICS_INFO:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_CONVERT_TO_CHARACTER_SPACE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_TO_FLOATS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_DOT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_ORIENTATIONINFREEFALL:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_HIT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_ROLLDOWNSTAIRS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_APPLYIMPULSE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_CONTACTREPORTER:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_DISTANCE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_CROSSPRODUCT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_RATE_OF_CHANGE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_ANGLE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_NORMALISE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_PHYSICALCONSTRAINT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_FALLOVERWALL:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_RETARGET:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_SCALE_CHARACTER:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_GUN_AIM_IK:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_TRANSIT_SYNC_EVENTS:
			nodeExport = Node::exportTransitSyncEventsNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS:
			nodeExport = Node::exportTransitSyncEventsNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_TRANSIT:
			nodeExport = Node::exportTransitNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_TRANSIT_PHYSICS:
			nodeExport = Node::exportTransitNode(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_MODIFY_JOINT_TRANSFORM:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_MODIFY_TRAJECTORY_TRANSFORM:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_SMOOTH_TRANSFORMS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_EMIT_MESSAGE_AND_CP:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_TRAJECTORY_OVERRIDE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_C_C_OVERRIDE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_LIMB_INFO:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_EXPAND_LIMITS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_COND_ON_MESSAGE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_COND_CP_COMPARE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		case NODE_TYPE_CP_OP_TIME_LAG_CP:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		default:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef);
			break;
		}

		Node::exportNodeTransitions(nodeDef, nodeExport);

		return nodeExport;
	}
}