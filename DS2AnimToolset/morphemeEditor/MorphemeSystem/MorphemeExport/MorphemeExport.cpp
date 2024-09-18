#include "MorphemeExport.h"
#include "RCore.h"
#include "extern.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/mrCharacterControllerDef.h"
#include "morpheme/mrMirroredAnimMapping.h"
#include "morpheme/Nodes/mrNodeFeatherBlend2.h"
#include "morpheme/Nodes/mrNodeFeatherBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeMirrorTransforms.h"
#include "assetProcessor/include/assetProcessor/BlendNodeBuilderUtils.h"

namespace
{
	int getConditionIndexByID(MR::StateDef* stateDef, int conditionID)
	{
		for (size_t i = 0; i < stateDef->getNumExitConditions(); i++)
		{
			if (stateDef->getExitConditionStateMachineIndex(i) == conditionID)
				return i;
		}

		return -1;
	}

	void setActiveStateTransitReferences(MR::AttribDataStateMachineDef* stateMachineDef, ME::NodeExportXML* nodeExport)
	{
		MR::StateDef* globalStateDef = stateMachineDef->getGlobalStateDef();

		int numConditions = globalStateDef->getNumExitConditions();

		for (size_t i = 0; i < numConditions; i++)
		{
			int conditionIndex = globalStateDef->getExitConditionStateMachineIndex(i);
			MR::TransitConditionDef* transitCondDef = stateMachineDef->getConditionDef(conditionIndex);

			MorphemeExport::TransitExport::exportTransitCommonCondition(nodeExport, transitCondDef);
		}

		for (int i = 0; i < globalStateDef->getNumExitTransitionStates(); i++)
		{
			MR::StateDef* transitStateDef = stateMachineDef->getStateDef(globalStateDef->getExitTransitionStateID(i));

			assert(transitStateDef->getNumEntryConditions() != 0);

			//TODO: This needs to be fixed, the indexes are in local space relative to the number of previously exported conditions, not in StateMachine space
			std::vector<unsigned int> indices;
			indices.reserve(numConditions);
			for (int j = 0; j < transitStateDef->getNumEntryConditions(); j++)
			{
				int conditionID = transitStateDef->getEntryConditionStateMachineIndex(j);
				indices.push_back(getConditionIndexByID(globalStateDef, conditionID));
			}

			//This is supposed to be a global state. If this condition is not met then I'm doing something wrong
			assert(transitStateDef->getTransitSourceStateID() == MR::INVALID_NODE_ID);

			int targetNodeID = stateMachineDef->getStateDef(transitStateDef->getTransitDestinationStateID())->getNodeID();
			MorphemeExport::TransitExport::exportTransitCommonConditionSet(nodeExport, targetNodeID, indices);
		}
	}

	void setTransitReferences(MR::NodeDef* nodeDef, ME::NodeExportXML* nodeExport)
	{
		MR::NetworkDef* netDef = nodeDef->getOwningNetworkDef();

		MR::NodeDef* parent = nodeDef->getParentNodeDef();

		while (true)
		{
			if (parent == nullptr || (parent->getNodeTypeID() == NODE_TYPE_STATE_MACHINE))
				break;

			parent = parent->getParentNodeDef();
		}

		if (parent == nullptr)
			return;

		MR::AttribDataStateMachineDef* stateMachineDef = static_cast<MR::AttribDataStateMachineDef*>(parent->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

		MR::StateDef* targetStateDef = nullptr;
		for (int i = 0; i < stateMachineDef->getNumStates(); i++)
		{
			MR::StateDef* stateDef = stateMachineDef->getStateDef(i);

			if (stateDef->getNodeID() == nodeDef->getNodeID())
			{
				targetStateDef = stateDef;
				break;
			}
		}

		if (targetStateDef)
		{
			int numConditions = targetStateDef->getNumExitConditions();

			for (size_t i = 0; i < numConditions; i++)
			{
				int conditionIndex = targetStateDef->getExitConditionStateMachineIndex(i);
				MR::TransitConditionDef* transitCondDef = stateMachineDef->getConditionDef(conditionIndex);

				MorphemeExport::TransitExport::exportTransitCommonCondition(nodeExport, transitCondDef);
			}

			for (int i = 0; i < targetStateDef->getNumExitTransitionStates(); i++)
			{
				MR::StateDef* transitStateDef = stateMachineDef->getStateDef(targetStateDef->getExitTransitionStateID(i));

				assert(transitStateDef->getNumEntryConditions() != 0);

				//TODO: This needs to be fixed, the indexes are in local space relative to the number of previously exported conditions, not in StateMachine space
				std::vector<unsigned int> indices;
				indices.reserve(numConditions);
				for (int j = 0; j < transitStateDef->getNumEntryConditions(); j++)
				{
					int conditionID = transitStateDef->getEntryConditionStateMachineIndex(j);
					indices.push_back(getConditionIndexByID(targetStateDef, conditionID));
				}

				//This is supposed to be a global state. If this condition is not met then I'm doing something wrong
				assert(transitStateDef->getTransitSourceStateID() == MR::INVALID_NODE_ID);

				int targetNodeID = stateMachineDef->getStateDef(transitStateDef->getTransitDestinationStateID())->getNodeID();
				MorphemeExport::TransitExport::exportTransitCommonConditionSet(nodeExport, targetNodeID, indices);
			}
		}
	}

	void writeDurationBlendFlags(MR::AttribDataUInt* durationEventMatchingOpAttrib, ME::DataBlockExportXML* attribDataBlock)
	{
		bool durationEventBlendPassThrough = false;
		bool durationEventBlendInSequence = false;
		bool durationEventBlendSameUserData = false;
		bool durationEventBlendOnOverlap = false;
		bool durationEventBlendWithinRange = false;

		if (durationEventMatchingOpAttrib)
		{
			switch (durationEventMatchingOpAttrib->m_value)
			{
			case DURATION_EVENT_MATCH_PASS_THROUGH:
				durationEventBlendPassThrough = true;
				break;
			case DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS_ON_OVERLAP:
				durationEventBlendInSequence = true;
				durationEventBlendSameUserData = true;
				durationEventBlendOnOverlap = true;
				break;
			case DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS_WITHIN_RANGE:
				durationEventBlendInSequence = true;
				durationEventBlendSameUserData = true;
				durationEventBlendWithinRange = true;
				break;
			case DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS:
				durationEventBlendInSequence = true;
				durationEventBlendSameUserData = true;
				break;
			case DURATION_EVENT_MATCH_IN_SEQUENCE_ON_OVERLAP:
				durationEventBlendInSequence = true;
				durationEventBlendOnOverlap = true;
				break;
			case DURATION_EVENT_MATCH_IN_SEQUENCE_WITHIN_RANGE:
				durationEventBlendInSequence = true;
				durationEventBlendWithinRange = true;
				break;
			case DURATION_EVENT_MATCH_IN_SEQUENCE:
				durationEventBlendInSequence = true;
				break;
			case DURATION_EVENT_MATCH_SAME_IDS_ON_OVERLAP:
				durationEventBlendSameUserData = true;
				durationEventBlendOnOverlap = true;
				break;
			case DURATION_EVENT_MATCH_SAME_IDS_WITHIN_RANGE:
				durationEventBlendSameUserData = true;
				durationEventBlendWithinRange = true;
				break;
			case DURATION_EVENT_MATCH_SAME_IDS:
				durationEventBlendSameUserData = true;
				break;
			case DURATION_EVENT_MATCH_ON_OVERLAP:
				durationEventBlendOnOverlap = true;
				break;
			case DURATION_EVENT_MATCH_WITHIN_RANGE:
				durationEventBlendWithinRange = true;
				break;
			default:
				g_appLog->panicMessage("Invalid blend matching operation %d\n", durationEventMatchingOpAttrib->m_value);
				break;
			}
		}

		attribDataBlock->writeBool(durationEventBlendPassThrough, "DurationEventBlendPassThrough");
		attribDataBlock->writeBool(durationEventBlendInSequence, "DurationEventBlendInSequence");
		attribDataBlock->writeBool(durationEventBlendSameUserData, "DurationEventBlendSameUserData");
		attribDataBlock->writeBool(durationEventBlendOnOverlap, "DurationEventBlendOnOverlap");
		attribDataBlock->writeBool(durationEventBlendWithinRange, "DurationEventBlendWithinRange");
	}

	void writeTimeStretchMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
	{
		int timeStretchMode = AP::kNodeTimeStretchNone;

		MR::AttribDataUInt* startSyncEventIndex = static_cast<MR::AttribDataUInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX));
		MR::AttribDataUInt* durationEventMatchingOpAttrib = static_cast<MR::AttribDataUInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP));
		MR::AttribDataBool* loop = static_cast<MR::AttribDataBool*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_LOOP));

		if (startSyncEventIndex && durationEventMatchingOpAttrib && loop)
			timeStretchMode = AP::kNodeTimeStretchMatchEvents;

		attribDataBlock->writeInt(timeStretchMode, "TimeStretchMode");

		if (loop)
			attribDataBlock->writeBool(loop->m_value, "Loop");
		else
			attribDataBlock->writeBool(true, "Loop");

		writeDurationBlendFlags(durationEventMatchingOpAttrib, attribDataBlock);

		if (startSyncEventIndex)
			attribDataBlock->writeInt(startSyncEventIndex->m_value, "StartEventIndex");
		else
			attribDataBlock->writeInt(0, "StartEventIndex");
	}

	void writeFeatherBlendModeFlags(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
	{
		AP::NodeBlendModes blendMode = AP::kNodeBlendInvalid;

		MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
		const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);
		
		g_appLog->debugMessage(MsgLevel_Debug, "\tATTRIB_SEMANTIC_TRANSFORM_BUFFER fn = %s\n", fnName);

		if (taskQueueFn == MR::nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttAddPos)
			blendMode == AP::kAddQuatAddPos;
		else if (taskQueueFn == MR::nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttInterpPos)
			blendMode == AP::kAddQuatInterpPos;
		else if (taskQueueFn == MR::nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttAddPos)
			blendMode == AP::kInterpQuatAddPos;
		else if (taskQueueFn == MR::nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttInterpPos)
			blendMode == AP::kInterpQuatInterpPos;
		else
			g_appLog->panicMessage("Unexpected ATTRIB_SEMANTIC_TRANSFORM_BUFFER queueing fn %s\n", fnName);

		bool additiveBlendAtt = false;
		bool additiveBlendPos = false;

		switch (blendMode)
		{
		case AP::kInterpQuatInterpPos:
			additiveBlendAtt = false;
			additiveBlendPos = false;
			break;
		case AP::kInterpQuatAddPos:
			additiveBlendAtt = false;
			additiveBlendPos = true;
			break;
		case AP::kAddQuatInterpPos:
			additiveBlendAtt = true;
			additiveBlendPos = false;
			break;
		case AP::kAddQuatAddPos:
			additiveBlendAtt = true;
			additiveBlendPos = true;
			break;
		default:
			break;
		}

		attribDataBlock->writeBool(additiveBlendAtt, "AdditiveBlendAttitude");
		attribDataBlock->writeBool(additiveBlendPos, "AdditiveBlendPosition");
	}

	void writeBlend2BlendModeFlags(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
	{
		AP::NodeBlendModes blendMode = AP::kNodeBlendInvalid;

		MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
		const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

		g_appLog->debugMessage(MsgLevel_Debug, "\tATTRIB_SEMANTIC_TRANSFORM_BUFFER fn = %s\n", fnName);

		if (taskQueueFn == MR::nodeBlend2QueueBlend2TransformBuffsAddAttAddPos)
			blendMode = AP::kAddQuatAddPos;
		else if (taskQueueFn == MR::nodeBlend2QueueBlend2TransformBuffsAddAttInterpPos)
			blendMode = AP::kAddQuatInterpPos;
		else if (taskQueueFn == MR::nodeBlend2QueueBlend2TransformBuffsInterpAttAddPos)
			blendMode = AP::kInterpQuatAddPos;
		else if (taskQueueFn == MR::nodeBlend2QueueBlend2TransformBuffsInterpAttInterpPos)
			blendMode = AP::kInterpQuatInterpPos;
		else
			g_appLog->panicMessage("Unexpected ATTRIB_SEMANTIC_TRANSFORM_BUFFER queueing fn %s\n", fnName);

		attribDataBlock->writeInt(blendMode, "BlendMode");
	}

	void writeNodeEventBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
	{
		AP::NodeSampledEventBlendModes eventBlendMode = AP::kSampledEventBlendModeInvalid;

		MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
		const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

		g_appLog->debugMessage(MsgLevel_Debug, "\tATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER fn = %s\n", fnName);

		if (taskQueueFn == MR::nodeBlend2SyncEventsQueueSampledEventsBuffers)
			eventBlendMode = AP::kMergeSampledEvents;
		else if (taskQueueFn == MR::nodeBlend2SyncEventsQueueAddSampledEventsBuffers)
			eventBlendMode = AP::kAddSampledEvents;
		else if (taskQueueFn == MR::nodeBlend2QueueSampledEventsBuffers)
			eventBlendMode = AP::kMergeSampledEvents;
		else if (taskQueueFn == MR::nodeBlend2QueueAddSampledEventsBuffers)
			eventBlendMode = AP::kAddSampledEvents;
		else if (taskQueueFn == MR::queuePassThroughChild0)
			eventBlendMode = AP::kMergeSampledEvents;
		else
			g_appLog->panicMessage("Unexpected task queing function %s\n", fnName);

		attribDataBlock->writeInt(eventBlendMode, "EventsBlendMode");
	}

	void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
	{
		bool slerpTrajPos = false;

		MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
		const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

		g_appLog->debugMessage(MsgLevel_Debug, "\tATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM fn = %s\n", fnName);

		if ((taskQueueFn == MR::nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttSlerpPos) || (taskQueueFn == MR::nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttSlerpPos))
			slerpTrajPos = true;

		attribDataBlock->writeBool(slerpTrajPos, "SphericallyInterpolateTrajectoryPosition");
	}

	void writePassThroughMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
	{
		short passThroughChildIndex = nodeDef->getPassThroughChildIndex();

		switch (passThroughChildIndex)
		{
		case 0:
			attribDataBlock->writeInt(AP::kNodePassThroughSource0, "PassThroughMode");
			break;
		case 1:
			attribDataBlock->writeInt(AP::kNodePassThroughSource1, "PassThroughMode");
			break;
		default:
			attribDataBlock->writeInt(AP::kNodePassThroughNone, "PassThroughMode");
			break;
		}
	}

	void writeNumAnimSets(ME::DataBlockExportXML* attribDataBlock, int numAnimSets)
	{
		attribDataBlock->writeInt(numAnimSets, "NumAnimSets");
	}

	bool isBlendNodeWrapWeights(MR::NodeDef* nodeDef)
	{
		int numSourceNodes = nodeDef->getNumChildNodes();

		MR::AttribDataFloatArray* weights = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

		if (weights->m_numValues == numSourceNodes)
			return false;

		return true;
	}

	void writeMirrorTransformEventPassThrough(ME::DataBlockExportXML* attribDataBlock, MR::NodeDef* nodeDef)
	{
		bool eventPassThrough = false;

		MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TIME_POS);
		const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

		if (taskQueueFn == MR::nodeMirrorQueueTimePos)
			eventPassThrough = true;

		attribDataBlock->writeBool(eventPassThrough, "EventPassThrough");
	}
}

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

namespace MorphemeExport
{
	ME::TakeListXML* exportAnimMarkup(MorphemeCharacterDef* character, int animId, std::wstring dstFileName)
	{
		MR::NetworkDef* netDef = character->getNetworkDef();

		MR::NodeDef* animNode = getAnimNodeByAnimID(netDef, animId);

		if (animNode == nullptr)
			return nullptr;

		MR::AttribDataBool* loopAttr = static_cast<MR::AttribDataBool*>(animNode->getAttribData(ATTRIB_SEMANTIC_LOOP));
		MR::AttribDataSourceAnim* sourceAnim = static_cast<MR::AttribDataSourceAnim*>(animNode->getAttribData(ATTRIB_SEMANTIC_SOURCE_ANIM));
		MR::AttribDataSourceEventTrackSet* sourceEvents = static_cast<MR::AttribDataSourceEventTrackSet*>(animNode->getAttribData(ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS));

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
			MR::AttribDataMirroredAnimMapping* mirroredMapping = static_cast<MR::AttribDataMirroredAnimMapping*>(netDef->getNodeDef(0)->getAttribData(ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING));

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
				MR::AttribDataMirroredAnimMapping* mirroredMapping = static_cast<MR::AttribDataMirroredAnimMapping*>(netDef->getNodeDef(0)->getAttribData(ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING));

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
			NodeExport::exportNode(netDefExport, netDef, i);

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

	namespace TransitExport
	{
		void createConditionExportDataBlock(ME::ConditionExportXML* conditionExport, MR::TransitConditType type)
		{
			switch (type)
			{
			case TRANSCOND_ON_MESSAGE_ID:

				break;
			case TRANSCOND_DISCRETE_EVENT_TRIGGERED_ID:
				// Handle case for TRANSCOND_DISCRETE_EVENT_TRIGGERED_ID
				break;
			case TRANSCOND_CROSSED_DURATION_FRACTION_ID:
				// Handle case for TRANSCOND_CROSSED_DURATION_FRACTION_ID
				break;
			case TRANSCOND_CROSSED_SYNC_EVENT_BOUNDARY_ID:
				// Handle case for TRANSCOND_CROSSED_SYNC_EVENT_BOUNDARY_ID
				break;
			case TRANSCOND_IN_SYNC_EVENT_SEGMENT_ID:
				// Handle case for TRANSCOND_IN_SYNC_EVENT_SEGMENT_ID
				break;
			case TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID
				break;
			case TRANSCOND_FALSE_ID:
				// Handle case for TRANSCOND_FALSE_ID
				break;
			case TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID
				break;
			case TRANSCOND_CONTROL_PARAM_IN_RANGE_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_IN_RANGE_ID
				break;
			case TRANSCOND_CROSSED_CURVE_EVENT_VALUE_DECREASING_ID:
				// Handle case for TRANSCOND_CROSSED_CURVE_EVENT_VALUE_DECREASING_ID
				break;
			case TRANSCOND_IN_SYNC_EVENT_RANGE_ID:
				// Handle case for TRANSCOND_IN_SYNC_EVENT_RANGE_ID
				break;
			case TRANSCOND_PHYSICS_AVAILABLE_ID:
				// Handle case for TRANSCOND_PHYSICS_AVAILABLE_ID
				break;
			case TRANSCOND_PHYSICS_IN_USE_ID:
				// Handle case for TRANSCOND_PHYSICS_IN_USE_ID
				break;
			case TRANSCOND_GROUND_CONTACT_ID:
				// Handle case for TRANSCOND_GROUND_CONTACT_ID
				break;
			case TRANSCOND_RAY_HIT_ID:
				// Handle case for TRANSCOND_RAY_HIT_ID
				break;
			case TRANSCOND_CONTROL_PARAM_TEST_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_TEST_ID
				break;
			case TRANSCOND_NODE_ACTIVE_ID:
				// Handle case for TRANSCOND_NODE_ACTIVE_ID
				break;
			case TRANSCOND_IN_DURATION_EVENT_ID:
				// Handle case for TRANSCOND_IN_DURATION_EVENT_ID
				break;
			case TRANSCOND_PHYSICS_MOVING_ID:
				// Handle case for TRANSCOND_PHYSICS_MOVING_ID
				break;
			case TRANSCOND_SK_DEVIATION_ID:
				// Handle case for TRANSCOND_SK_DEVIATION_ID
				break;
			case TRANSCOND_CONTROL_PARAM_UINT_GREATER_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_UINT_GREATER_ID
				break;
			case TRANSCOND_CONTROL_PARAM_UINT_LESS_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_UINT_LESS_ID
				break;
			case TRANSCOND_CONTROL_PARAM_INT_GREATER_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_INT_GREATER_ID
				break;
			case TRANSCOND_CONTROL_PARAM_INT_LESS_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_INT_LESS_ID
				break;
			case TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID
				break;
			case TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID
				break;
			case TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID
				break;
			case TRANSCOND_CONTROL_PARAM_BOOL_SET_ID:
				// Handle case for TRANSCOND_CONTROL_PARAM_BOOL_SET_ID
				break;
			default:
				// Handle default case
				break;
			}
		}

		ME::ConditionSetExportXML* exportTransitConditionSet(ME::NodeExportXML* nodeExport, int targetNodeID, std::vector<unsigned int>& indices)
		{
			int idx = nodeExport->getNumConditionSets();
			ME::ConditionSetExportXML* conditionSet = static_cast<ME::ConditionSetExportXML*>(nodeExport->createConditionSet(idx, targetNodeID, indices));

			return conditionSet;
		}

		ME::ConditionExportXML* exportTransitCondition(ME::NodeExportXML* nodeExport, MR::TransitConditionDef* transitCondDef)
		{
			int idx = nodeExport->getNumConditions();

			MR::TransitConditType type = transitCondDef->getType();
			ME::ConditionExportXML* conditionExport = static_cast<ME::ConditionExportXML*>(nodeExport->createCondition(idx, type));

			createConditionExportDataBlock(conditionExport, type);

			return conditionExport;
		}

		ME::ConditionSetExportXML* exportTransitCommonConditionSet(ME::NodeExportXML* nodeExport, int targetNodeID, std::vector<unsigned int>& indices)
		{
			int idx = nodeExport->getNumCommonConditionSets();
			ME::ConditionSetExportXML* conditionSet = static_cast<ME::ConditionSetExportXML*>(nodeExport->createCommonConditionSet(idx, targetNodeID, indices));

			return conditionSet;
		}

		ME::ConditionExportXML* exportTransitCommonCondition(ME::NodeExportXML* nodeExport, MR::TransitConditionDef* transitCondDef)
		{
			int idx = nodeExport->getNumCommonConditions();

			MR::TransitConditType type = transitCondDef->getType();
			ME::ConditionExportXML* conditionExport = static_cast<ME::ConditionExportXML*>(nodeExport->createCommonCondition(idx, type));

			createConditionExportDataBlock(conditionExport, type);

			return conditionExport;
		}
	}
	
	namespace NodeExport
	{
		ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, int nodeId)
		{
			MR::NodeDef* nodeDef = netDef->getNodeDef(nodeId);
			MR::NodeType nodeTypeID = nodeDef->getNodeTypeID();

			g_appLog->debugMessage(MsgLevel_Debug, "Exporting node %d (typeId=%d)\n", nodeId, nodeTypeID);

			ME::NodeExportXML* nodeExport = nullptr;

			switch (nodeTypeID)
			{
			case NODE_TYPE_NETWORK:
				nodeExport = exportNetworkNode(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_STATE_MACHINE:
				nodeExport = exportStateMachineNode(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_FLOAT:
				nodeExport = exportCPFloatNode(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_VECTOR3:
				nodeExport = exportCPVector3Node(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_VECTOR4:
				nodeExport = exportCPVector4Node(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_BOOL:
				nodeExport = exportCPBoolNode(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_INT:
				nodeExport = exportCPIntNode(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_UINT:
				nodeExport = exportCPUIntNode(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_PHYSICS_OBJECT_POINTER:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_ANIMATION:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_ANIM_EVENTS:
				nodeExport = exportAnimSyncEventsNode(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_FILTER_TRANSFORMS:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_BLEND_2:
				nodeExport = exportBlend2Node(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_BLEND_N:
				nodeExport = exportBlendNNode(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_SINGLEFRAME:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_FREEZE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_BLEND_2X2:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_BLEND_NXM:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_BLEND_ALL:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_SUBTRACTIVE_BLEND:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_SCATTER_BLEND_1D:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_SCATTER_BLEND_2D:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_SCATTER_BLEND_3D:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_BOOLEAN:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_FUNCTION:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_ARITHMETIC:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_ARITHMETIC_VECTOR3:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_NOISE_GEN:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_FEATHER_BLEND_2:
				nodeExport = exportFeatherBlend2Node(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_APPLY_BIND_POSE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_PHYSICS_GROUPER:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_BEHAVIOUR_GROUPER:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_APPLY_GLOBAL_TIME:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_APPLY_PHYSICS_JOINT_LIMITS:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_SET_NON_PHYSICS_TRANSFORMS:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_TWO_BONE_IK:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_LOCK_FOOT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_HEAD_LOOK:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_PHYSICS:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_BEHAVIOUR:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_HIPS_IK:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_PERFORMANCE_BEHAVIOUR:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_PLAY_SPEED_MODIFIER:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_SCALE_TO_DURATION:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_EMIT_JOINT_CP_INFO:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CLOSEST_ANIM:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_SWITCH:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_SEQUENCE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_PASSTHROUGH:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_MIRROR_TRANSFORMS_ID:
				nodeExport = exportMirrorTransformNode(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_BASIC_UNEVEN_TERRAIN:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_ACTIVE_STATE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_INT_TO_FLOAT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_RAY_CAST:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_SMOOTH_FLOAT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_RAMP_FLOAT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_FLOATS_TO_VECTOR3:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_FLOAT_TO_INT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_RANDOM_FLOAT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_PHYSICS_INFO:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_CONVERT_TO_CHARACTER_SPACE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_VECTOR3_TO_FLOATS:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_VECTOR3_DOT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_ORIENTATIONINFREEFALL:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_HIT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_ROLLDOWNSTAIRS:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_APPLYIMPULSE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_CONTACTREPORTER:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_VECTOR3_DISTANCE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_VECTOR3_CROSSPRODUCT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_RATE_OF_CHANGE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_VECTOR3_ANGLE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_VECTOR3_NORMALISE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_PHYSICALCONSTRAINT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_FALLOVERWALL:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_RETARGET:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_SCALE_CHARACTER:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_GUN_AIM_IK:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_TRANSIT_SYNC_EVENTS:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_TRANSIT:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_TRANSIT_PHYSICS:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_MODIFY_JOINT_TRANSFORM:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_MODIFY_TRAJECTORY_TRANSFORM:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_SMOOTH_TRANSFORMS:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_EMIT_MESSAGE_AND_CP:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_TRAJECTORY_OVERRIDE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_C_C_OVERRIDE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_LIMB_INFO:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_EXPAND_LIMITS:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_COND_ON_MESSAGE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_COND_CP_COMPARE:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			case NODE_TYPE_CP_OP_TIME_LAG_CP:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			default:
				nodeExport = exportNodeUnhandled(netDefExport, netDef, nodeDef);
				break;
			}

			setTransitReferences(nodeDef, nodeExport);

			return nodeExport;
		}

		ME::NodeExportXML* exportNodeCore(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			ME::NodeExportXML* nodeExportXML = static_cast<ME::NodeExportXML*>(netDefExport->createNode(nodeDef->getNodeID(), nodeDef->getNodeTypeID(), nodeDef->getParentNodeID(), false, netDef->getNodeNameFromNodeID(nodeDef->getNodeID())));
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeUInt(nodeDef->getNumOnEnterMessages(), "NumOnEnterInternalMessageIDs");
			nodeDataBlock->writeUInt(0, "NumOnEnterExternalMessageIDs");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportNodeUnhandled(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);

			g_appLog->debugMessage(MsgLevel_Warn, "Unhandled node exporter for node type %d\n", nodeDef->getNodeTypeID());

			return nodeExportXML;
		}

		ME::NodeExportXML* exportNetworkNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_NETWORK)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_NETWORK, nodeDef->getNodeTypeID());

			return exportNodeCore(netDefExport, netDef, nodeDef);
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

		void writeStateMachineStatesInfo(MR::NodeDef* nodeDef, ME::DataBlockExportXML* nodeDataBlock)
		{
			MR::NetworkDef* netDef = nodeDef->getOwningNetworkDef();

			std::vector<StateDef*> childNodes = getStateMachineNodeChildNodes(netDef, nodeDef);

			int childNodeCount = childNodes.size();
			nodeDataBlock->writeUInt(childNodeCount, "ChildNodeCount");

			if (childNodeCount)
			{
				for (int i = 0; i < childNodeCount; i++)
				{
					char paramName[256];
					sprintf_s(paramName, "RuntimeChildNodeID_%i", i);

					nodeDataBlock->writeNetworkNodeId(childNodes[i]->getNodeID(), paramName);
				}
			}

			std::vector<StateDef*> childTransitNodes = getStateMachineNodeChildTransitNodes(netDef, nodeDef);

			int childTransitCount = childTransitNodes.size();
			nodeDataBlock->writeUInt(childTransitCount, "ChildTransitionCount");

			if (childTransitCount)
			{
				for (int i = 0; i < childTransitCount; i++)
				{
					char paramName[256];
					sprintf_s(paramName, "RuntimeChildTransitID_%i", i);

					nodeDataBlock->writeNetworkNodeId(childTransitNodes[i]->getNodeID(), paramName);
				}
			}

			if ((childTransitCount + childNodeCount) != nodeDef->getNumChildNodes())
				g_appLog->panicMessage("Total parsed node count is different from the total node children count (expected %d, got %d)\n", nodeDef->getNumChildNodes(), childTransitCount + childNodeCount);
		}

		//TODO: Revise
		ME::NodeExportXML* exportStateMachineNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_STATE_MACHINE)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_STATE_MACHINE, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataStateMachineDef* stateMachineDef = static_cast<MR::AttribDataStateMachineDef*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			writeStateMachineStatesInfo(nodeDef, nodeDataBlock);

			int defaultStateID = stateMachineDef->getStateDef(stateMachineDef->getDefaultStartingStateID())->getNodeID();
			nodeDataBlock->writeUInt(defaultStateID, "DefaultNodeID");

			setActiveStateTransitReferences(stateMachineDef, nodeExportXML);

			return nodeExportXML;
		}

		ME::NodeExportXML* exportCPFloatNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_FLOAT)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_FLOAT, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataFloat* attribData = static_cast<MR::AttribDataFloat*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_FLOAT));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");

			nodeDataBlock->writeFloat(attribData->m_value, "DefaultValue_0");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportCPVector3Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_VECTOR3)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_VECTOR3, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataVector3* attribData = static_cast<MR::AttribDataVector3*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_VECTOR3));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");

			nodeDataBlock->writeFloat(attribData->m_value[0], "DefaultValue_0");
			nodeDataBlock->writeFloat(attribData->m_value[1], "DefaultValue_1");
			nodeDataBlock->writeFloat(attribData->m_value[2], "DefaultValue_2");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportCPVector4Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_VECTOR4)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_VECTOR4, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataVector4* attribData = static_cast<MR::AttribDataVector4*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_VECTOR4));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");

			nodeDataBlock->writeFloat(attribData->m_value[0], "DefaultValue_0");
			nodeDataBlock->writeFloat(attribData->m_value[1], "DefaultValue_1");
			nodeDataBlock->writeFloat(attribData->m_value[2], "DefaultValue_2");
			nodeDataBlock->writeFloat(attribData->m_value[3], "DefaultValue_3");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportCPBoolNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_BOOL)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_BOOL, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataBool* attribData = static_cast<MR::AttribDataBool*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_BOOL));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");
			nodeDataBlock->writeBool(attribData->m_value, "DefaultFlag");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportCPIntNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_INT)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_INT, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataInt* attribData = static_cast<MR::AttribDataInt*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_INT));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");
			nodeDataBlock->writeInt(attribData->m_value, "DefaultInt");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportCPUIntNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_UINT)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_UINT, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataUInt* attribData = static_cast<MR::AttribDataUInt*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CP_UINT));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");
			nodeDataBlock->writeUInt(attribData->m_value, "DefaultInt");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportAnimSyncEventsNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_ANIM_EVENTS)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_ANIM_EVENTS, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			int numAnimSets = netDef->getNumAnimSets();

			MR::AttribDataBool* isLoop = static_cast<MR::AttribDataBool*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_LOOP));

			for (short setIndex = 0; setIndex < netDef->getNumAnimSets(); setIndex++)
			{
				MR::AttribDataSourceAnim* sourceAnim = static_cast<MR::AttribDataSourceAnim*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_SOURCE_ANIM, setIndex));

				if (sourceAnim)
				{
					if (setIndex == 0)
					{
						nodeDataBlock->writeAnimationId(sourceAnim->m_animAssetID, "AnimIndex");
						nodeDataBlock->writeBool(false, "GenerateAnimationDeltas");
						nodeDataBlock->writeBool(sourceAnim->m_playBackwards, "PlayBackwards");
						nodeDataBlock->writeBool(isLoop->m_value, "Loop");
					}

					char paramName[256];

					sprintf_s(paramName, "DefaultClip_%d", setIndex + 1);
					nodeDataBlock->writeBool(true, paramName);

					sprintf_s(paramName, "ClipRangeMode_%d", setIndex + 1);
					nodeDataBlock->writeInt(3, paramName);

					sprintf_s(paramName, "ClipStartFraction_%d", setIndex + 1);
					nodeDataBlock->writeFloat(sourceAnim->m_clipStartFraction, paramName);

					sprintf_s(paramName, "ClipEndFraction_%d", setIndex + 1);
					nodeDataBlock->writeFloat(sourceAnim->m_clipEndFraction, paramName);

					sprintf_s(paramName, "StartEventIndex_%d", setIndex + 1);
					nodeDataBlock->writeFloat(sourceAnim->m_startSyncEventIndex, paramName);
				}
			}

			return nodeExportXML;
		}

		ME::NodeExportXML* exportMirrorTransformNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_MIRROR_TRANSFORMS_ID)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_MIRROR_TRANSFORMS_ID, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeInt(nodeDef->getChildNodeID(0), "InputNodeID");
			writeNumAnimSets(nodeDataBlock, numAnimSets);

			for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; animSetIndex++)
			{
				CHAR paramName[256];

				sprintf_s(paramName, "NonMirroredIdCount_%d", animSetIndex + 1); // We add one to the index as LUA arrays start at 1 and the manifest was written out using LUA array indices

				MR::AttribDataIntArray* unfilteredBonesArray = static_cast<MR::AttribDataIntArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_BONE_IDS, animSetIndex));

				nodeDataBlock->writeInt(unfilteredBonesArray->m_numValues, paramName);

				for (int k = 0; k < unfilteredBonesArray->m_numValues; k++)
				{
					sprintf_s(paramName, "Id_%d_%d", animSetIndex + 1, k + 1);
					nodeDataBlock->writeUInt(unfilteredBonesArray->m_values[k], paramName);
				}

				MR::AttribDataInt* eventOffset = static_cast<MR::AttribDataInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_SYNC_EVENT_OFFSET, animSetIndex));
				nodeDataBlock->writeUInt(eventOffset->m_value, "EventOffset");
			}

			writeMirrorTransformEventPassThrough(nodeDataBlock, nodeDef);

			return nodeExportXML;
		}

		ME::NodeExportXML* exportBlend2Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_BLEND_2)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_BLEND_2, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeInt(nodeDef->getChildNodeID(0), "Source0NodeID");
			nodeDataBlock->writeInt(nodeDef->getChildNodeID(1), "Source1NodeID");

			MR::AttribDataFloatArray* weights = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

			assert(weights->m_numValues == 2);

			nodeDataBlock->readFloat(weights->m_values[0], "BlendWeight_0");
			nodeDataBlock->readFloat(weights->m_values[1], "BlendWeight_1");

			writePassThroughMode(nodeDef, nodeDataBlock);
			writeNodeEventBlendMode(nodeDef, nodeDataBlock);
			writeSlerpTrajPos(nodeDef, nodeDataBlock);
			writeBlend2BlendModeFlags(nodeDef, nodeDataBlock);

			MR::AttribDataBlendFlags* blendFlags = static_cast<MR::AttribDataBlendFlags*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_BLEND_FLAGS));
			nodeDataBlock->writeBool(blendFlags->m_alwaysCombineSampledEvents, "AlwaysCombineSampledEvents");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportBlendNNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_BLEND_N)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_BLEND_N, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			int sourceNodeCount = nodeDef->getNumChildNodes();

			nodeDataBlock->writeInt(sourceNodeCount, "SourceNodeCount");

			CHAR paramName[256];
			for (int i = 0; i < sourceNodeCount; i++)
			{
				sprintf_s(paramName, "Source%dNodeID", i);

				nodeDataBlock->writeInt(nodeDef->getChildNodeID(i), paramName);
			}

			MR::AttribDataFloatArray* weights = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));
			for (int i = 0; i < sourceNodeCount; i++)
			{
				sprintf_s(paramName, "SourceWeight_%d", i);

				nodeDataBlock->writeFloat(weights->m_values[i], paramName);
			}

			bool wrapWeights = isBlendNodeWrapWeights(nodeDef);
			nodeDataBlock->writeBool(wrapWeights, "WrapWeights");

			if (wrapWeights)
				nodeDataBlock->writeFloat(weights->m_values[sourceNodeCount], "WrapWeight");

			writeTimeStretchMode(nodeDef, nodeDataBlock);
			writeSlerpTrajPos(nodeDef, nodeDataBlock);

			MR::AttribDataBlendFlags* blendFlags = static_cast<MR::AttribDataBlendFlags*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_BLEND_FLAGS));
			nodeDataBlock->writeBool(blendFlags->m_alwaysCombineSampledEvents, "AlwaysCombineSampledEvents");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportFeatherBlend2Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_FEATHER_BLEND_2)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_FEATHER_BLEND_2, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "Source0NodeID");
			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(1), "Source1NodeID");
			nodeDataBlock->writeNetworkNodeId(nodeDef->getInputCPConnectionSourceNodeID(0), "Weight");

			MR::AttribDataFloatArray* childWeights = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

			nodeDataBlock->writeFloat(childWeights->m_values[0], "BlendWeight_0");
			nodeDataBlock->writeFloat(childWeights->m_values[1], "BlendWeight_1");

			writePassThroughMode(nodeDef, nodeDataBlock);
			writeTimeStretchMode(nodeDef, nodeDataBlock);
			writeFeatherBlendModeFlags(nodeDef, nodeDataBlock);
			writeNodeEventBlendMode(nodeDef, nodeDataBlock);
			writeSlerpTrajPos(nodeDef, nodeDataBlock);

			MR::AttribDataBlendFlags* blendFlags = static_cast<MR::AttribDataBlendFlags*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_BLEND_FLAGS));
			nodeDataBlock->writeBool(blendFlags->m_alwaysCombineSampledEvents, "AlwaysCombineSampledEvents");

			writeNumAnimSets(nodeDataBlock, numAnimSets);

			char paramNumAlphas[256];
			char paramAlpha[256];
			for (short setIndex = 0; setIndex < numAnimSets; setIndex++)
			{
				MR::AttribDataFeatherBlend2ChannelAlphas* channelAlphas = static_cast<MR::AttribDataFeatherBlend2ChannelAlphas*>(nodeDef->getAttribData(ATTRIB_SEMANTIC_BONE_WEIGHTS, setIndex));

				int numAlphaValues = channelAlphas->m_numChannelAlphas;

				sprintf_s(paramNumAlphas, "ChannelAlphasSet%dCount", setIndex);
				nodeDataBlock->writeInt(numAlphaValues, paramNumAlphas);

				for (size_t j = 0; j < numAlphaValues; j++)
				{
					sprintf_s(paramAlpha, "ChannelAlphasSet%d_Value%d", setIndex, j);
					nodeDataBlock->writeFloat(channelAlphas->m_channelAlphas[j], paramAlpha);
				}
			}

			return nodeExportXML;
		}
	}
}