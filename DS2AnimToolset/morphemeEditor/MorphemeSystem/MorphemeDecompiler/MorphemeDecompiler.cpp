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
	ME::EventTrackExport* getExportedTrack(std::vector<ME::EventTrackExport*>& exportedTracks, MR::EventTrackDefBase* track)
	{
		ME::EventTrackExport* targetTrack = nullptr;
		MR::EventTrackDefDiscrete* trackDiscrete = static_cast<MR::EventTrackDefDiscrete*>(track);
		MR::EventTrackDefCurve* trackCurve = static_cast<MR::EventTrackDefCurve*>(track);
		MR::EventTrackDefDuration* trackDur = static_cast<MR::EventTrackDefDuration*>(track);

		switch (track->getType())
		{
		case MR::kEventType_Discrete:
			for (size_t trackIdx = 0; trackIdx < exportedTracks.size(); trackIdx++)
			{
				ME::DiscreteEventTrackExport* trackExport = static_cast<ME::DiscreteEventTrackExport*>(exportedTracks[trackIdx]);

				if (trackExport->getEventTrackType() != ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE)
					continue;
				
				if ((trackDiscrete->getTrackID() == trackExport->getEventTrackChannelID()) &&
					(trackDiscrete->getUserData() == trackExport->getUserData()) &&
					(trackDiscrete->getNumEvents() == trackExport->getNumEvents()) &&
					(strcmp(trackDiscrete->getTrackName(), trackExport->getName()) == 0))
				{
					targetTrack = trackExport;

					for (size_t eventIdx = 0; eventIdx < trackExport->getNumEvents(); eventIdx++)
					{
						ME::DiscreteEventExport* eventExport = trackExport->getEvent(eventIdx);

						if ((eventExport->getNormalisedTime() != trackDiscrete->getEvent(eventIdx)->getStartTime()) ||
							(eventExport->getUserData() != trackDiscrete->getEvent(eventIdx)->getUserData()))
						{
							return nullptr;
						}
					}

					if (targetTrack != nullptr)
						return targetTrack;
				}
			}
			break;
		case MR::kEventType_Curve:
			for (size_t trackIdx = 0; trackIdx < exportedTracks.size(); trackIdx++)
			{
				ME::CurveEventTrackExport* trackExport = static_cast<ME::CurveEventTrackExport*>(exportedTracks[trackIdx]);

				if (trackExport->getEventTrackType() != ME::EventTrackExport::EVENT_TRACK_TYPE_CURVE)
					continue;

				if ((trackCurve->getTrackID() == trackExport->getEventTrackChannelID()) &&
					(trackCurve->getUserData() == trackExport->getUserData()) &&
					(trackCurve->getNumEvents() == trackExport->getNumEvents()) &&
					(strcmp(trackCurve->getTrackName(), trackExport->getName()) == 0))
				{
					targetTrack = trackExport;

					for (size_t eventIdx = 0; eventIdx < trackExport->getNumEvents(); eventIdx++)
					{
						ME::CurveEventExport* eventExport = trackExport->getEvent(eventIdx);

						if ((eventExport->getNormalisedStartTime() != trackCurve->getEvent(eventIdx)->getTime()) ||
							(eventExport->getFloatValue() != trackCurve->getEvent(eventIdx)->getValue()) ||
							(eventExport->getUserData() != trackCurve->getEvent(eventIdx)->getUserData()))
						{
							return nullptr;
						}
					}

					if (targetTrack != nullptr)
						return targetTrack;
				}
			}
			break;
		case MR::kEventType_Duration:
			for (size_t trackIdx = 0; trackIdx < exportedTracks.size(); trackIdx++)
			{
				ME::DurationEventTrackExport* trackExport = static_cast<ME::DurationEventTrackExport*>(exportedTracks[trackIdx]);

				if (trackExport->getEventTrackType() != ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION)
					continue;

				if ((trackDur->getTrackID() == trackExport->getEventTrackChannelID()) &&
					(trackDur->getUserData() == trackExport->getUserData()) &&
					(trackDur->getNumEvents() == trackExport->getNumEvents()) &&
					(strcmp(trackDur->getTrackName(), trackExport->getName()) == 0))
				{
					targetTrack = trackExport;

					for (size_t eventIdx = 0; eventIdx < trackExport->getNumEvents(); eventIdx++)
					{
						ME::DurationEventExport* eventExport = trackExport->getEvent(eventIdx);

						if ((eventExport->getNormalisedStartTime() != trackDur->getEvent(eventIdx)->getStartTime()) ||
							(eventExport->getNormalisedDuration() != trackDur->getEvent(eventIdx)->getDuration()) ||
							(eventExport->getUserData() != trackDur->getEvent(eventIdx)->getUserData()))
						{
							return nullptr;
						}
					}

					if (targetTrack != nullptr)
						return targetTrack;
				}
			}
			break;
		default:
			break;
		}

		return targetTrack;
	}

	ME::TakeListXML* exportAnimMarkup(MorphemeCharacterDef* character, int animId, std::wstring dstFileName, std::vector<ME::EventTrackExport*>& exportedTracks)
	{
		MR::NetworkDef* netDef = character->getNetworkDef();

		MR::NodeDef* animNode = MorphemeUtils::getAnimNodeByAnimID(netDef, animId);

		ME::ExportFactoryXML factory;
		ME::TakeListXML* takeList = (ME::TakeListXML*)factory.createTakeList(dstFileName.c_str(), dstFileName.c_str());

		if (animNode == nullptr)
		{
			takeList->createTake(RString::toWide(character->getAnimFileLookUp()->getTakeName(animId)).c_str(), 1, 30, true);
			return takeList;
		}

		MR::AttribDataBool* loopAttr = static_cast<MR::AttribDataBool*>(animNode->getAttribData(MR::ATTRIB_SEMANTIC_LOOP));
		MR::AttribDataSourceAnim* sourceAnim = static_cast<MR::AttribDataSourceAnim*>(animNode->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM));
		MR::AttribDataSourceEventTrackSet* sourceEvents = static_cast<MR::AttribDataSourceEventTrackSet*>(animNode->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS));

		bool loop = false;

		if (loopAttr != nullptr)
			loop = loopAttr->m_value;

		ME::TakeExportXML* take = static_cast<ME::TakeExportXML*>(takeList->createTake(RString::toWide(character->getAnimFileLookUp()->getTakeName(animId)).c_str(), sourceAnim->m_sourceAnimDuration, 30, loop, sourceAnim->m_clipStartFraction, sourceAnim->m_clipEndFraction));

		for (size_t i = 0; i < sourceEvents->m_numDiscreteEventTracks; i++)
		{
			MR::EventTrackDefDiscrete* track = sourceEvents->m_sourceDiscreteEventTracks[i];
			ME::EventTrackExport* targetTrack = getExportedTrack(exportedTracks, track);

			std::string guid;

			if (targetTrack == nullptr)
			{
				GUID gidReference;
				CoCreateGuid(&gidReference);

				guid = RString::guidToString(gidReference);
			}
			else
				guid = targetTrack->getGUID();

			ME::DiscreteEventTrackExportXML* trackXML = (ME::DiscreteEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE, guid.c_str(), RString::toWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

			for (size_t j = 0; j < track->getNumEvents(); j++)
				trackXML->createEvent(j, track->getEvent(j)->getStartTime(), track->getEvent(j)->getUserData());

			if (targetTrack == nullptr)
				exportedTracks.push_back(trackXML);
		}

		for (size_t i = 0; i < sourceEvents->m_numCurveEventTracks; i++)
		{
			MR::EventTrackDefCurve* track = sourceEvents->m_sourceCurveEventTracks[i];
			ME::EventTrackExport* targetTrack = getExportedTrack(exportedTracks, track);
			std::string guid;

			if (targetTrack == nullptr)
			{
				GUID gidReference;
				CoCreateGuid(&gidReference);

				guid = RString::guidToString(gidReference);
			}
			else
				guid = targetTrack->getGUID();

			ME::CurveEventTrackExportXML* trackXML = (ME::CurveEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_CURVE, guid.c_str(), RString::toWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

			for (size_t j = 0; j < track->getNumEvents(); j++)
				trackXML->createEvent(j, track->getEvent(j)->getTime(), track->getEvent(j)->getValue(), track->getEvent(j)->getUserData());

			if (targetTrack == nullptr)
				exportedTracks.push_back(trackXML);
		}

		for (size_t i = 0; i < sourceEvents->m_numDurEventTracks; i++)
		{
			MR::EventTrackDefDuration* track = sourceEvents->m_sourceDurEventTracks[i];
			ME::EventTrackExport* targetTrack = getExportedTrack(exportedTracks, track);
			
			std::string guid;
			
			if (targetTrack == nullptr)
			{
				GUID gidReference;
				CoCreateGuid(&gidReference);

				guid = RString::guidToString(gidReference);
			}
			else
				guid = targetTrack->getGUID();

			ME::DurationEventTrackExportXML* trackXML = (ME::DurationEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION, guid.c_str(), RString::toWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

			for (size_t j = 0; j < track->getNumEvents(); j++)
				trackXML->createEvent(j, track->getEvent(j)->getStartTime(), track->getEvent(j)->getDuration(), track->getEvent(j)->getUserData());
		
			if (targetTrack == nullptr)
				exportedTracks.push_back(trackXML);
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
		MR::NodeDef* nodeDef = netDef->getNodeDef(nodeId);
		MR::NodeType nodeTypeID = nodeDef->getNodeTypeID();

		g_appLog->debugMessage(MsgLevel_Info, "\tExporting node %d (name=\"%s\", typeId=%d)\n", nodeId, nodeName.c_str(), nodeTypeID);

		ME::NodeExportXML* nodeExport = nullptr;

		switch (nodeTypeID)
		{
		case NODE_TYPE_NETWORK:
			nodeExport = Node::exportNetworkNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_STATE_MACHINE:
			nodeExport = Node::exportStateMachineNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_FLOAT:
			nodeExport = Node::exportCPFloatNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_VECTOR3:
			nodeExport = Node::exportCPVector3Node(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_VECTOR4:
			nodeExport = Node::exportCPVector4Node(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_BOOL:
			nodeExport = Node::exportCPBoolNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_INT:
			nodeExport = Node::exportCPIntNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_UINT:
			nodeExport = Node::exportCPUIntNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_PHYSICS_OBJECT_POINTER:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_ANIMATION:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_ANIM_EVENTS:
			nodeExport = Node::exportAnimSyncEventsNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_FILTER_TRANSFORMS:
			nodeExport = Node::exportFilterTransformsNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_BLEND_2:
			nodeExport = Node::exportBlend2Node(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_BLEND_N:
			nodeExport = Node::exportBlendNNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_SINGLEFRAME:
			nodeExport = Node::exportSingleFrameNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_FREEZE:
			nodeExport = Node::exportFreezeNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_BLEND_2X2:
			nodeExport = Node::exportBlend2x2Node(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_BLEND_NXM:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_BLEND_ALL:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_SUBTRACTIVE_BLEND:
			nodeExport = Node::exportSubtractiveBlendNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_SCATTER_BLEND_1D:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_SCATTER_BLEND_2D:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_SCATTER_BLEND_3D:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_BOOLEAN:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_FUNCTION:
			nodeExport = Node::exportOperatorFunctionNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_ARITHMETIC:
			nodeExport = Node::exportOperatorArithmeticNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_ARITHMETIC_VECTOR3:
			nodeExport = Node::exportOperatorArithmeticVector3Node(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC:
			nodeExport = Node::exportOperatorOneInputArithmeticNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3:
			nodeExport = Node::exportOperatorOneInputArithmeticVector3Node(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_NOISE_GEN:
			nodeExport = Node::exportOperatorNoiseGenNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_FEATHER_BLEND_2:
			nodeExport = Node::exportFeatherBlendNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_APPLY_BIND_POSE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_PHYSICS_GROUPER:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_BEHAVIOUR_GROUPER:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_APPLY_GLOBAL_TIME:
			nodeExport = Node::exportApplyGlobalTimeNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_APPLY_PHYSICS_JOINT_LIMITS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_SET_NON_PHYSICS_TRANSFORMS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_TWO_BONE_IK:
			nodeExport = Node::exportTwoBoneIKNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_LOCK_FOOT:
			nodeExport = Node::exportLockFootNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_HEAD_LOOK:
			nodeExport = Node::exportHeadLookNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_PHYSICS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_BEHAVIOUR:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_HIPS_IK:
			nodeExport = Node::exportHipsIKNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_PERFORMANCE_BEHAVIOUR:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_PLAY_SPEED_MODIFIER:
			nodeExport = Node::exportPlaySpeedModiferNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_SCALE_TO_DURATION:
			nodeExport = Node::exportScaleToDuration(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_EMIT_JOINT_CP_INFO:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT:
			nodeExport = Node::exportEmitRequestOnDiscreteEventNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CLOSEST_ANIM:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_SWITCH:
			nodeExport = Node::exportSwitchNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_SEQUENCE:
			nodeExport = Node::exportSequenceNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_PASSTHROUGH:
			nodeExport = Node::exportPassThroughNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_MIRROR_TRANSFORMS_ID:
			nodeExport = Node::exportMirrorTransformNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_BASIC_UNEVEN_TERRAIN:
			nodeExport = Node::exportBasicUnevenTerrainNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN:
			nodeExport = Node::exportPredictiveUnevenTerrainNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_INT_TO_FLOAT:
			nodeExport = Node::exportOperatorIntToFloatNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_RAY_CAST:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_SMOOTH_FLOAT:
			nodeExport = Node::exportOperatorSmoothFloatNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_RAMP_FLOAT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_FLOATS_TO_VECTOR3:
			nodeExport = Node::exportOperatorFloatsToVector3Node(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_FLOAT_TO_INT:
			nodeExport = Node::exportOperatorFloatToIntNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_RANDOM_FLOAT:
			nodeExport = Node::exportOperatorRandomFloatNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_PHYSICS_INFO:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_CONVERT_TO_CHARACTER_SPACE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_TO_FLOATS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_DOT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_ORIENTATIONINFREEFALL:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_HIT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_ROLLDOWNSTAIRS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_APPLYIMPULSE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_CONTACTREPORTER:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_DISTANCE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_CROSSPRODUCT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_RATE_OF_CHANGE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_ANGLE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_VECTOR3_NORMALISE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_PHYSICALCONSTRAINT:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_FALLOVERWALL:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_RETARGET:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_SCALE_CHARACTER:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_GUN_AIM_IK:
			nodeExport = Node::exportGunAimIKNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_TRANSIT_SYNC_EVENTS:
			nodeExport = Node::exportTransitSyncEventsNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS:
			nodeExport = Node::exportTransitSyncEventsNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_TRANSIT:
			nodeExport = Node::exportTransitNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_TRANSIT_PHYSICS:
			nodeExport = Node::exportTransitNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_MODIFY_JOINT_TRANSFORM:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_MODIFY_TRAJECTORY_TRANSFORM:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_SMOOTH_TRANSFORMS:
			nodeExport = Node::exportSmoothTransformsNode(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_EMIT_MESSAGE_AND_CP:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_TRAJECTORY_OVERRIDE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_C_C_OVERRIDE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_LIMB_INFO:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_EXPAND_LIMITS:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_COND_ON_MESSAGE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_COND_CP_COMPARE:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		case NODE_TYPE_CP_OP_TIME_LAG_CP:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		default:
			nodeExport = Node::exportNodeUnhandled(netDefExport, netDef, nodeDef, nodeName);
			break;
		}

		Node::exportNodeTransitions(nodeDef, nodeExport);

		return nodeExport;
	}
}