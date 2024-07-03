#include "MorphemeExport.h"
#include "utils/RString/RString.h"
#include "utils/RLog/RLog.h"
#include "utils/RMath/RMath.h"
#include "extern.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/mrCharacterControllerDef.h"
#include "morpheme/mrMirroredAnimMapping.h"

using namespace MR;

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

ME::TakeListXML* MorphemeExport::ExportAnimMarkup(CharacterDef* character, int animId, std::wstring dstFileName)
{
	MR::NetworkDef* netDef = character->getNetworkDef();

	MR::NodeDef* animNode = getAnimNodeByAnimID(netDef, animId);

	if (animNode == nullptr)
		return nullptr;

	MR::AttribDataSourceAnim* sourceAnim = (MR::AttribDataSourceAnim*)animNode->getAttribData(ATTRIB_SEMANTIC_SOURCE_ANIM);
	MR::AttribDataSourceEventTrackSet* sourceEvents = (MR::AttribDataSourceEventTrackSet*)animNode->getAttribData(ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS);

	int sourceAnimId = sourceAnim->m_animAssetID;

	ME::ExportFactoryXML factory;
	std::wstring dest = dstFileName;

	ME::TakeListXML* takeList = (ME::TakeListXML*)factory.createTakeList(dest.c_str(), dest.c_str());
	ME::TakeExportXML* take = (ME::TakeExportXML*)takeList->createTake(RString::ToWide(character->getAnimFileLookUp()->getTakeName(sourceAnimId)).c_str(), sourceAnim->m_sourceAnimDuration, 30, false, sourceAnim->m_clipStartFraction, sourceAnim->m_clipEndFraction);

	for (size_t i = 0; i < sourceEvents->m_numDiscreteEventTracks; i++)
	{
		GUID gidReference;
		CoCreateGuid(&gidReference);

		MR::EventTrackDefDiscrete* track = sourceEvents->m_sourceDiscreteEventTracks[i];

		ME::DiscreteEventTrackExportXML* trackXML = (ME::DiscreteEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE, RString::GuidToString(gidReference).c_str(), RString::ToWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

		for (size_t j = 0; j < track->getNumEvents(); j++)
			trackXML->createEvent(j, track->getEvent(j)->getStartTime(), track->getEvent(j)->getUserData());
	}

	for (size_t i = 0; i < sourceEvents->m_numCurveEventTracks; i++)
	{
		GUID gidReference;
		CoCreateGuid(&gidReference);

		MR::EventTrackDefCurve* track = sourceEvents->m_sourceCurveEventTracks[i];

		ME::CurveEventTrackExportXML* trackXML = (ME::CurveEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_CURVE, RString::GuidToString(gidReference).c_str(), RString::ToWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

		for (size_t j = 0; j < track->getNumEvents(); j++)
			trackXML->createEvent(j, track->getEvent(j)->getTime(), track->getEvent(j)->getValue(), track->getEvent(j)->getUserData());
	}

	for (size_t i = 0; i < sourceEvents->m_numDurEventTracks; i++)
	{
		GUID gidReference;
		CoCreateGuid(&gidReference);

		MR::EventTrackDefDuration* track = sourceEvents->m_sourceDurEventTracks[i];

		ME::DurationEventTrackExportXML* trackXML = (ME::DurationEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION, RString::GuidToString(gidReference).c_str(), RString::ToWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

		for (size_t j = 0; j < track->getNumEvents(); j++)
			trackXML->createEvent(j, track->getEvent(j)->getStartTime(), track->getEvent(j)->getDuration(), track->getEvent(j)->getUserData());
	}

	return takeList;
}

ME::CharacterControllerExportXML* MorphemeExport::ExportCharacterController(MR::CharacterControllerDef* characterController, std::wstring dstFileName)
{
	ME::ExportFactoryXML factory;

	GUID gidReference;
	CoCreateGuid(&gidReference);

	ME::CharacterControllerExportXML* characterControllerExport = static_cast<ME::CharacterControllerExportXML*>(factory.createCharacterController(RString::GuidToString(gidReference).c_str(), dstFileName.c_str(), "Controller"));

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
		attributes->createDoubleAttribute("MaxSlopeAngle", RMath::ConvertDegAngleToRad(characterController->getMaxSlopeAngle()));
		attributes->createDoubleAttribute("StepHeight", characterController->getStepHeight());
	}

	return characterControllerExport;
}

ME::RigExportXML* MorphemeExport::ExportRig(MR::NetworkDef* netDef, MR::AnimRigDef* rig, std::wstring dstFileName)
{
	ME::ExportFactoryXML factory;

	GUID gidReference;
	CoCreateGuid(&gidReference);

	ME::RigExportXML* rigExport = static_cast<ME::RigExportXML*>(factory.createRig(RString::GuidToString(gidReference).c_str(), dstFileName.c_str(), RString::RemoveExtension(RString::ToNarrow(dstFileName.c_str())).c_str()));

	for (size_t i = 0; i < rig->getNumBones(); i++)
	{
		ME::JointExportXML* jointExport = static_cast<ME::JointExportXML*>(rigExport->createJoint(i, RString::ToWide(rig->getBoneName(i)).c_str(), rig->getParentBoneIndex(i)));

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

	MR::AttribDataMirroredAnimMapping* mirroredMapping = static_cast<MR::AttribDataMirroredAnimMapping*>(netDef->getNodeDef(0)->getAttribData(ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING));

	for (size_t i = 0; i < mirroredMapping->getNumMappings(); i++)
		rigExport->createJointMapping(i, mirroredMapping->getLeftBone(i), mirroredMapping->getRightBone(i));

	return rigExport;
}

ME::AnimationLibraryXML* MorphemeExport::ExportAnimLibrary(MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* filenameLookup, MR::NetworkDef* netDef, std::vector<ME::RigExportXML*> rigExport, std::vector<ME::CharacterControllerExportXML*> controllerExport, int chrId, std::wstring dstFileName)
{
	ME::ExportFactoryXML factory;

	GUID gidReference;
	CoCreateGuid(&gidReference);

	ME::AnimationLibraryXML* animLibraryExport = static_cast<ME::AnimationLibraryXML*>(factory.createAnimationLibrary(RString::GuidToString(gidReference).c_str(), L"ConnectLibrary", dstFileName.c_str()));

	for (size_t i = 0; i < netDef->getNumAnimSets(); i++)
	{
		wchar_t setName[260];
		swprintf_s(setName, L"%04d_%d", chrId, i);

		ME::AnimationSetExportXML* animSetExport = static_cast<ME::AnimationSetExportXML*>(animLibraryExport->createAnimationSet(i, setName));
		
		if (controllerExport[i])
			animSetExport->setCharacterController(controllerExport[i]);

		if (rigExport[i])
			animSetExport->setRig(rigExport[i]);

		MR::AttribDataMirroredAnimMapping* mirroredMapping = static_cast<MR::AttribDataMirroredAnimMapping*>(netDef->getNodeDef(0)->getAttribData(ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING));

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

			animSetExport->createAnimationEntry(i, RString::ToWide(sourceFileName).c_str(), takeFile, RString::ToWide(filenameLookup->getTakeName(i)).c_str(), L"Footsteps", "nsa", "");
		}
	}

	return animLibraryExport;
}

ME::MessagePresetLibraryExportXML* MorphemeExport::ExportMessagePresetLibrary(MR::NetworkDef* netDef, int chrId, std::wstring dstFileName)
{
	ME::ExportFactoryXML factory;

	GUID gidReference;
	CoCreateGuid(&gidReference);

	ME::MessagePresetLibraryExportXML* messagePresetLibraryExport = static_cast<ME::MessagePresetLibraryExportXML*>(factory.createMessagePresetLibrary(RString::GuidToString(gidReference).c_str(), dstFileName.c_str()));

	return messagePresetLibraryExport;
}

ME::NetworkDefExportXML* MorphemeExport::ExportNetwork(MR::NetworkDef* netDef, ME::AnimationLibraryXML* animLibraryExport, ME::MessagePresetLibraryExportXML* messagePresetLibraryExport, int chrId, std::wstring dstFileName)
{
	ME::ExportFactoryXML factory;

	GUID gidReference;
	CoCreateGuid(&gidReference);

	wchar_t netName[260];
	swprintf_s(netName, L"c%04d", chrId);

	ME::NetworkDefExportXML* netDefExport = static_cast<ME::NetworkDefExportXML*>(factory.createNetworkDef(RString::GuidToString(gidReference).c_str(), netName, dstFileName.c_str()));

	netDefExport->setNetworkWorldOrientation(NMP::Vector3XAxis(), NMP::Vector3ZAxis(), NMP::Vector3XAxis());

	for (size_t i = 0; i < netDef->getNumNodeDefs(); i++)
		MorphemeExport::ExportNode(netDefExport, netDef, i);

	const NMP::IDMappedStringTable* messageTable = netDef->getMessageIDNamesTable();

	for (size_t i = 0; i < netDef->getNumMessages(); i++)
	{
		int messageId = netDef->getMessageDistributor(i)->m_messageID;
		int messageType = netDef->getMessageDistributor(i)->m_messageType;

		netDefExport->createMessage(RString::ToWide(netDef->getMessageNameFromMessageID(messageId)).c_str(), messageType, messageId);
	}

	netDefExport->setRootNodeNetworkID(netDef->getRootNodeID());

	if (animLibraryExport)
		netDefExport->setAnimationLibrary(animLibraryExport);

	if (messagePresetLibraryExport)
		netDefExport->setMessagePresetLibrary(messagePresetLibraryExport);

	return netDefExport;
}

ME::NodeExportXML* MorphemeExport::ExportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, int nodeId)
{
	MR::NodeDef* nodeDef = netDef->getNodeDef(nodeId);

	MR::NodeType nodeTypeID = nodeDef->getNodeTypeID();

    switch (nodeTypeID) 
    {
    case NODE_TYPE_NETWORK:
        return MorphemeExport::ExportNetworkNode(netDefExport, netDef, nodeDef);
    case NODE_TYPE_STATE_MACHINE:
        return MorphemeExport::ExportStateMachineNode(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_FLOAT:
        return MorphemeExport::ExportCPFloatNode(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_VECTOR3:
        return MorphemeExport::ExportCPVector3Node(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_VECTOR4:
        return MorphemeExport::ExportCPVector4Node(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_BOOL:
        return MorphemeExport::ExportCPBoolNode(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_INT:
        return MorphemeExport::ExportCPIntNode(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_UINT:
        return MorphemeExport::ExportCPUIntNode(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_PHYSICS_OBJECT_POINTER:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_ANIMATION:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_ANIM_EVENTS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_FILTER_TRANSFORMS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_BLEND_2:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_BLEND_N:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_SINGLEFRAME:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_FREEZE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_BLEND_2X2:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_BLEND_NXM:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_BLEND_ALL:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_SUBTRACTIVE_BLEND:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_SCATTER_BLEND_1D:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_SCATTER_BLEND_2D:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_SCATTER_BLEND_3D:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_BOOLEAN:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_FUNCTION:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_ARITHMETIC:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_ARITHMETIC_VECTOR3:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_NOISE_GEN:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_FEATHER_BLEND_2:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_APPLY_BIND_POSE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_PHYSICS_GROUPER:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_BEHAVIOUR_GROUPER:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_APPLY_GLOBAL_TIME:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_APPLY_PHYSICS_JOINT_LIMITS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_SET_NON_PHYSICS_TRANSFORMS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_TWO_BONE_IK:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_LOCK_FOOT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_HEAD_LOOK:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_PHYSICS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_BEHAVIOUR:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_HIPS_IK:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_PERFORMANCE_BEHAVIOUR:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_PLAY_SPEED_MODIFIER:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_SCALE_TO_DURATION:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_EMIT_JOINT_CP_INFO:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CLOSEST_ANIM:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_SWITCH:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_SEQUENCE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_PASSTHROUGH:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_MIRROR_TRANSFORMS_ID:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_BASIC_UNEVEN_TERRAIN:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_ACTIVE_STATE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_INT_TO_FLOAT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_RAY_CAST:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_SMOOTH_FLOAT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_RAMP_FLOAT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_FLOATS_TO_VECTOR3:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_FLOAT_TO_INT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_RANDOM_FLOAT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_PHYSICS_INFO:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_CONVERT_TO_CHARACTER_SPACE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_VECTOR3_TO_FLOATS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_VECTOR3_DOT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_ORIENTATIONINFREEFALL:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_HIT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_ROLLDOWNSTAIRS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_APPLYIMPULSE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_CONTACTREPORTER:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_VECTOR3_DISTANCE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_VECTOR3_CROSSPRODUCT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_RATE_OF_CHANGE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_VECTOR3_ANGLE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_VECTOR3_NORMALISE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_PHYSICALCONSTRAINT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_FALLOVERWALL:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_RETARGET:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_SCALE_CHARACTER:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_GUN_AIM_IK:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_TRANSIT_SYNC_EVENTS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_TRANSIT:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_TRANSIT_PHYSICS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_MODIFY_JOINT_TRANSFORM:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_MODIFY_TRAJECTORY_TRANSFORM:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_SMOOTH_TRANSFORMS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_EMIT_MESSAGE_AND_CP:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_TRAJECTORY_OVERRIDE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_C_C_OVERRIDE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_LIMB_INFO:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_EXPAND_LIMITS:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_COND_ON_MESSAGE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_COND_CP_COMPARE:
        return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    case NODE_TYPE_CP_OP_TIME_LAG_CP:
		return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    default:
		return MorphemeExport::ExportNodeUnhandled(netDefExport, netDef, nodeDef);
    }
}

ME::NodeExportXML* MorphemeExport::ExportNodeCore(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
	ME::NodeExportXML* nodeExportXML = static_cast<ME::NodeExportXML*>(netDefExport->createNode(nodeDef->getNodeID(), nodeDef->getNodeTypeID(), nodeDef->getParentNodeID(), false, netDef->getNodeNameFromNodeID(nodeDef->getNodeID())));
	ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

	return nodeExportXML;
}

ME::NodeExportXML* MorphemeExport::ExportNodeUnhandled(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
    ME::NodeExportXML* nodeExportXML = ExportNodeCore(netDefExport, netDef, nodeDef);

    g_appLog->DebugMessage(MsgLevel_Warn, "Unhandled node exporter for node type %d\n", nodeDef->getNodeTypeID());

    return nodeExportXML;
}

ME::NodeExportXML* MorphemeExport::ExportNetworkNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
	if (nodeDef->getNodeTypeID() != NODE_TYPE_NETWORK)
		g_appLog->PanicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_NETWORK, nodeDef->getNodeTypeID());

	return ExportNodeCore(netDefExport, netDef, nodeDef);
}

std::vector<StateDef*> getStateMachineNodeChildNodes(MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
	std::vector<StateDef*> childStates;

	MR::AttribDataStateMachineDef* attribData = static_cast<MR::AttribDataStateMachineDef*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

	if (attribData == nullptr)
		return childStates;

	int childStateCount = nodeDef->getNumChildNodes();
	
	for (size_t i = 0; i < childStateCount; i++)
	{
		StateDef* stateDef = attribData->getStateDef(i);

		int nodeID = stateDef->getNodeID();

		MR::NodeDef* def = netDef->getNodeDef(nodeID);

		int nodeType = def->getNodeTypeID();

		if ((nodeType != NODE_TYPE_TRANSIT) && (nodeType != NODE_TYPE_TRANSIT_PHYSICS) && (nodeType != NODE_TYPE_TRANSIT_SYNC_EVENTS) && (nodeType != NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS))
			childStates.push_back(stateDef);
	}

	return childStates;
}

std::vector<StateDef*> getStateMachineNodeChildTransitNodes(MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
	std::vector<StateDef*> childStates;

	MR::AttribDataStateMachineDef* attribData = static_cast<MR::AttribDataStateMachineDef*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

	if (attribData == nullptr)
		return childStates;

	int childStateCount = nodeDef->getNumChildNodes();

	for (size_t i = 0; i < childStateCount; i++)
	{
		StateDef* stateDef = attribData->getStateDef(i);

		int nodeID = stateDef->getNodeID();

		MR::NodeDef* def = netDef->getNodeDef(nodeID);

		int nodeType = def->getNodeTypeID();

		if ((nodeType == NODE_TYPE_TRANSIT) || (nodeType == NODE_TYPE_TRANSIT_PHYSICS) || (nodeType == NODE_TYPE_TRANSIT_SYNC_EVENTS) || (nodeType == NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS))
			childStates.push_back(stateDef);
	}

	return childStates;
}

ME::NodeExportXML* MorphemeExport::ExportStateMachineNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
	if (nodeDef->getNodeTypeID() != NODE_TYPE_STATE_MACHINE)
		g_appLog->PanicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_STATE_MACHINE, nodeDef->getNodeTypeID());

	ME::NodeExportXML* nodeExportXML = ExportNodeCore(netDefExport, netDef, nodeDef);
	ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

	MR::AttribDataStateMachineDef* attribData = static_cast<MR::AttribDataStateMachineDef*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

	std::vector<StateDef*> childNodes = getStateMachineNodeChildNodes(netDef, nodeDef);

	int childNodeCount = childNodes.size();
	nodeDataBlock->writeUInt(childNodeCount, "ChildNodeCount");

	if (childNodeCount)
	{
		for (size_t i = 0; i < childNodeCount; i++)
		{
			char paramName[256];
			sprintf_s(paramName, "RuntimeChildNodeID_%i", i);

			nodeDataBlock->writeNetworkNodeId(childNodes[i]->getNodeID(), paramName);
		}
	}

	std::vector<StateDef*> childTransitNodes = getStateMachineNodeChildTransitNodes(netDef, nodeDef);

	int childTransitCount = childTransitNodes.size();
	nodeDataBlock->writeUInt(childTransitCount, "ChildTransitCount");

	if (childTransitCount)
	{
		for (size_t i = 0; i < childTransitCount; i++)
		{
			char paramName[256];
			sprintf_s(paramName, "RuntimeChildTransitID_%i", i);

			nodeDataBlock->writeNetworkNodeId(childTransitNodes[i]->getNodeID(), paramName);
		}
	}

	int defaultStateID = attribData->getStateDef(attribData->getDefaultStartingStateID())->getNodeID();
	nodeDataBlock->writeUInt(defaultStateID, "DefaultNodeID");

	if ((childTransitCount + childNodeCount) != nodeDef->getNumChildNodes())
		g_appLog->PanicMessage("Total parsed node count is different from the total node children count (expected %d, got %d)\n", nodeDef->getNumChildNodes(), childTransitCount + childNodeCount);
	
	return nodeExportXML;
}

ME::NodeExportXML* MorphemeExport::ExportCPFloatNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
	if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_FLOAT)
		g_appLog->PanicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_FLOAT, nodeDef->getNodeTypeID());

	ME::NodeExportXML* nodeExportXML = ExportNodeCore(netDefExport, netDef, nodeDef);
	ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

	MR::AttribDataFloat* attribData = static_cast<MR::AttribDataFloat*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_FLOAT));

	if (attribData == nullptr)
		return nullptr;

	nodeDataBlock->writeFloat(attribData->m_value, "DefaultValue_0");

	return nodeExportXML;
}

ME::NodeExportXML* MorphemeExport::ExportCPVector3Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
	if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_VECTOR3)
		g_appLog->PanicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_VECTOR3, nodeDef->getNodeTypeID());

	ME::NodeExportXML* nodeExportXML = ExportNodeCore(netDefExport, netDef, nodeDef);
	ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

	MR::AttribDataVector3* attribData = static_cast<MR::AttribDataVector3*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_VECTOR3));

	if (attribData == nullptr)
		return nullptr;

	nodeDataBlock->writeFloat(attribData->m_value[0], "DefaultValue_0");
	nodeDataBlock->writeFloat(attribData->m_value[1], "DefaultValue_1");
	nodeDataBlock->writeFloat(attribData->m_value[2], "DefaultValue_2");

	return nodeExportXML;
}

ME::NodeExportXML* MorphemeExport::ExportCPVector4Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
	if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_VECTOR4)
		g_appLog->PanicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_VECTOR4, nodeDef->getNodeTypeID());

	ME::NodeExportXML* nodeExportXML = ExportNodeCore(netDefExport, netDef, nodeDef);
	ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

	MR::AttribDataVector4* attribData = static_cast<MR::AttribDataVector4*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_VECTOR4));

	if (attribData == nullptr)
		return nullptr;

	nodeDataBlock->writeFloat(attribData->m_value[0], "DefaultValue_0");
	nodeDataBlock->writeFloat(attribData->m_value[1], "DefaultValue_1");
	nodeDataBlock->writeFloat(attribData->m_value[2], "DefaultValue_2");
	nodeDataBlock->writeFloat(attribData->m_value[3], "DefaultValue_3");

	return nodeExportXML;
}

ME::NodeExportXML* MorphemeExport::ExportCPBoolNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
	if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_BOOL)
		g_appLog->PanicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_BOOL, nodeDef->getNodeTypeID());

	ME::NodeExportXML* nodeExportXML = ExportNodeCore(netDefExport, netDef, nodeDef);
	ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

	MR::AttribDataBool* attribData = static_cast<MR::AttribDataBool*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_BOOL));

	if (attribData == nullptr)
		return nullptr;

	nodeDataBlock->writeBool(attribData->m_value, "DefaultFlag");

	return nodeExportXML;
}

ME::NodeExportXML* MorphemeExport::ExportCPIntNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
	if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_INT)
		g_appLog->PanicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_INT, nodeDef->getNodeTypeID());

	ME::NodeExportXML* nodeExportXML = ExportNodeCore(netDefExport, netDef, nodeDef);
	ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

	MR::AttribDataInt* attribData = static_cast<MR::AttribDataInt*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_INT));

	if (attribData == nullptr)
		return nullptr;

	nodeDataBlock->writeInt(attribData->m_value, "DefaultInt");

	return nodeExportXML;
}

ME::NodeExportXML* MorphemeExport::ExportCPUIntNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
{
	if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_UINT)
		g_appLog->PanicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_UINT, nodeDef->getNodeTypeID());

	ME::NodeExportXML* nodeExportXML = ExportNodeCore(netDefExport, netDef, nodeDef);
	ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

	MR::AttribDataUInt* attribData = static_cast<MR::AttribDataUInt*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_UINT));

	if (attribData == nullptr)
		return nullptr;

	nodeDataBlock->writeUInt(attribData->m_value, "DefaultInt");

	return nodeExportXML;
}