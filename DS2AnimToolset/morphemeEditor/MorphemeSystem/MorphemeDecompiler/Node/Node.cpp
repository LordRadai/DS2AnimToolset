#include "Node.h"
#include "morpheme/Nodes/mrNodeMirrorTransforms.h"
#include "../TransitCondition/TransitCondition.h"
#include "../../MorphemeUtils/MorphemeUtils.h"
#include "extern.h"
#include "RCore.h"
#include "morpheme/Nodes/mrNodeFreeze.h"
#include "morpheme/Nodes/mrNodeExtractJointInfo.h"
#include "NodeUtils.h"
#include "StateMachineNode.h"
#include "ControlParamNode.h"
#include "BlendNode.h"
#include "TransitNode.h"
#include "IKNode.h"
#include "OperatorNode.h"

namespace MD
{
	namespace Node
	{
		bool NodeExtractJointInfoDecompiler::getOutputSpace(MR::NodeDef* nodeDef)
		{
			MR::QueueAttrTaskFn transformBufferFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

			if ((transformBufferFn == MR::nodeExtractJointInfoObjectQueueTransforms) || (transformBufferFn == MR::nodeExtractJointInfoObjectJointSelectQueueTransforms))
				return true;
			else if ((transformBufferFn == MR::nodeExtractJointInfoLocalQueueTransforms) || (transformBufferFn == MR::nodeExtractJointInfoLocalJointSelectQueueTransforms))
				return false;
			else
				g_appLog->panicMessage("Invalid transform buffer function for ExtractJointInfo node %s (nodeID=%d)\n", MR::Manager::getInstance().getTaskQueuingFnName(transformBufferFn), nodeDef->getNodeID());
		}

		bool NodeFreezeDecompiler::isPassThroughTransformsOnce(MR::NodeDef* nodeDef)
		{
			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
			const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

			//g_appLog->debugMessage(MsgLevel_Debug, "\tATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER fn = %s\n", fnName);

			if (taskQueueFn == MR::nodeFreezePassThroughLastTransformsOnce)
				return true;
			else if (taskQueueFn == MR::nodeFreezePassThroughLastTransforms)
				return false;
			else
				g_appLog->panicMessage("Unexpected task queing function %s\n", fnName);

			return false;
		}

		ME::NodeExportXML* NodeUnhandledDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);

			g_appLog->debugMessage(MsgLevel_Warn, "\tUnhandled node exporter for node type %d\n", nodeDef->getNodeTypeID());
			
			std::ofstream unhandledDump;

			char line[256];
			unhandledDump.open("unhandledNodes.txt", std::ios::out | std::ios::app);

			sprintf_s(line, "%d (typeID=%d)\n", nodeDef->getNodeID(), nodeDef->getNodeTypeID());

			unhandledDump << line;
			unhandledDump.close();

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeNetworkDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_NETWORK);

			return NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
		}

		ME::NodeExportXML* NodeAnimSyncEventsDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_ANIM_EVENTS);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			int numAnimSets = netDef->getNumAnimSets();

			MR::AttribDataBool* isLoop = static_cast<MR::AttribDataBool*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_LOOP));

			for (short setIndex = 0; setIndex < netDef->getNumAnimSets(); setIndex++)
			{
				MR::AttribDataSourceAnim* sourceAnim = static_cast<MR::AttribDataSourceAnim*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM, setIndex));

				if (setIndex == 0)
				{
					MR::AttribDataSyncEventTrack* syncEventTrack = static_cast<MR::AttribDataSyncEventTrack*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, setIndex));

					nodeDataBlock->writeAnimationId(sourceAnim->m_animAssetID, "AnimIndex");
					nodeDataBlock->writeBool(false, "GenerateAnimationDeltas");
					nodeDataBlock->writeBool(isLoop->m_value, "Loop");
					nodeDataBlock->writeBool(sourceAnim->m_playBackwards, "PlayBackwards");
					nodeDataBlock->writeBool(syncEventTrack != nullptr, "PreComputeSyncEventTracks");
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
				nodeDataBlock->writeUInt(sourceAnim->m_startSyncEventIndex, paramName);
			}

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeMirrorTransformsDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_MIRROR_TRANSFORMS_ID);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			nodeDataBlock->writeInt(numAnimSets, "NumAnimSets");

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

				MR::AttribDataInt* attribDataEventOffset = static_cast<MR::AttribDataInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_SYNC_EVENT_OFFSET, animSetIndex));
				
				int eventOffset = 0;
				if (attribDataEventOffset != nullptr)
					eventOffset = attribDataEventOffset->m_value;

				nodeDataBlock->writeUInt(eventOffset, "EventOffset");
			}

			bool eventPassThrough = true;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TIME_POS);
			const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

			if (taskQueueFn == MR::nodeMirrorQueueTimePos)
				eventPassThrough = false;

			nodeDataBlock->writeBool(eventPassThrough, "EventPassThrough");

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeFilterTransformsDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_FILTER_TRANSFORMS);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "NodeConnectedTo");
			nodeDataBlock->writeInt(numAnimSets, "NumAnimSets"); 

			for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; animSetIndex++)
			{
				CHAR paramName[256];

				MR::AttribDataUIntArray* numBoneFilterIDs = static_cast<MR::AttribDataUIntArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_BONE_IDS, animSetIndex));

				sprintf_s(paramName, "FilterIdCount_%d", animSetIndex + 1);
				nodeDataBlock->writeInt(numBoneFilterIDs->m_numValues, paramName);

				for (uint32_t k = 0; k < numBoneFilterIDs->m_numValues; k++)
				{
					sprintf_s(paramName, "Id_%d_%d", animSetIndex + 1, k + 1);
					nodeDataBlock->writeUInt(numBoneFilterIDs->m_values[k], paramName);
				}
			}

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeSingleFrameDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_SINGLEFRAME);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Control", nodeDef->getInputCPConnection(0), true);

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeFreezeDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_FREEZE);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeBool(isPassThroughTransformsOnce(nodeDef), "passThroughTransformsOnce");

			return nodeExportXML;
		}

		MR::AttribDataEmittedMessagesMap::RequestType NodeEmitRequestOnDiscreteEventDecompiler::getRequestTypeID(MR::AttribDataEmittedMessagesMap* emittedMessageMap, uint32_t index)
		{
			if ((emittedMessageMap->getEmittedMessageEntry(index)->stateMachineNodeID == MR::INVALID_NODE_ID) && (emittedMessageMap->getEmittedMessageEntry(index)->messageID == MR::INVALID_MESSAGE_ID))
				return MR::AttribDataEmittedMessagesMap::UNUSED;
			else if ((emittedMessageMap->getEmittedMessageEntry(index)->stateMachineNodeID == MR::INVALID_NODE_ID) && (emittedMessageMap->getEmittedMessageEntry(index)->messageID != MR::INVALID_MESSAGE_ID))
				return MR::AttribDataEmittedMessagesMap::EXTERNAL;
			else if (!emittedMessageMap->isEntryUsed(index))
				return MR::AttribDataEmittedMessagesMap::CLEAR;
			else if ((emittedMessageMap->getEmittedMessageEntry(index)->stateMachineNodeID != MR::INVALID_NODE_ID) && (emittedMessageMap->getEmittedMessageEntry(index)->messageID != MR::INVALID_MESSAGE_ID))
				return MR::AttribDataEmittedMessagesMap::SET;
			else if ((emittedMessageMap->getEmittedMessageEntry(index)->stateMachineNodeID != MR::INVALID_NODE_ID) && (emittedMessageMap->getEmittedMessageEntry(index)->messageID == MR::INVALID_MESSAGE_ID))
				return MR::AttribDataEmittedMessagesMap::RESET;
			else
				return MR::AttribDataEmittedMessagesMap::UNUSED;
		}

		ME::NodeExportXML* NodeEmitRequestOnDiscreteEventDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "SourceNodeID");

			MR::AttribDataEmittedMessagesMap* emittedMessageMap = static_cast<MR::AttribDataEmittedMessagesMap*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES_MAP));
			MR::AttribDataIntArray* eventUserDatas = static_cast<MR::AttribDataIntArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			int numMessageSlots = emittedMessageMap->getEmittedMessageMapLength();

			nodeDataBlock->writeUInt(numMessageSlots, "NumMessageSlots");

			CHAR paramName[256];
			for (uint32_t i = 0; i < numMessageSlots; i++)
			{
				const MR::EmittedMessageMap* messageMap = emittedMessageMap->getEmittedMessageEntry(i);

				sprintf_s(paramName, "EventUserData_%d", i);
				nodeDataBlock->writeInt(eventUserDatas->m_values[i], paramName);

				MR::AttribDataEmittedMessagesMap::RequestType actionID = getRequestTypeID(emittedMessageMap, i);

				sprintf_s(paramName, "ActionID_%d", i);
				nodeDataBlock->writeUInt(actionID, paramName);
				
				if (actionID != MR::AttribDataEmittedMessagesMap::UNUSED)
				{
					sprintf_s(paramName, "EmittedMessageID_%d", i);
					nodeDataBlock->writeUInt(messageMap->messageID, paramName);

					bool broadcast = false;

					if (messageMap->stateMachineNodeID == MR::NETWORK_NODE_ID)
						broadcast = true;

					sprintf_s(paramName, "Broadcast_%d", i);
					nodeDataBlock->writeBool(broadcast, paramName);

					if (!broadcast)
					{
						sprintf_s(paramName, "TargetNodePath_%d", i);
						nodeDataBlock->writeString(netDefExport->getNode(messageMap->stateMachineNodeID)->getName(), paramName);
					}
				}			
			}

			nodeDataBlock->writeBool(true, "NodeEmitsMessages");

			return nodeExportXML;
		}

		ME::NodeExportXML* NodePlaySpeedModifierDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_PLAY_SPEED_MODIFIER);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Weight", nodeDef->getInputCPConnection(0), true);

			return nodeExportXML;
		}

		ME::NodeExportXML* NodePassThroughDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_PASSTHROUGH);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "NodeConnectedTo");

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeSmoothTransformsDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_SMOOTH_TRANSFORMS);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "SourceNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Multiplier", nodeDef->getInputCPConnection(0), true);

			int numAnimSets = netDef->getNumAnimSets();

			CHAR paramNumSmoothingStrenght[256];
			CHAR paramSmoothingStrenght[256];
			for (uint32_t animSetIdx = 0; animSetIdx < numAnimSets; animSetIdx++)
			{
				sprintf_s(paramNumSmoothingStrenght, "numSmoothingStrengthsSet_%d", animSetIdx + 1);

				MR::AttribDataFloatArray* smoothingStrenghtAttribData = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_BONE_WEIGHTS, animSetIdx));
				nodeDataBlock->writeUInt(smoothingStrenghtAttribData->m_numValues, paramNumSmoothingStrenght);

				for (uint32_t i = 0; i < smoothingStrenghtAttribData->m_numValues; i++)
				{
					sprintf_s(paramSmoothingStrenght, "SmoothingStrengths_%d_Set_%d", i + 1, animSetIdx + 1);
					nodeDataBlock->writeFloat(smoothingStrenghtAttribData->m_values[i], paramSmoothingStrenght);
				}
			}

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeSwitchDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_SWITCH);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int childNodeCount = nodeDef->getNumChildNodes();

			nodeDataBlock->writeInt(childNodeCount, "SourceNodeCount");

			CHAR paramName[256];
			for (uint32_t i = 0; i < childNodeCount; i++)
			{
				sprintf_s(paramName, "Source%dNodeID", i);
				nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(i), paramName);
			}

			NodeUtils::writeInputCPConnection(nodeDataBlock, "Weight", nodeDef->getInputCPConnection(0), true);

			MR::AttribDataSwitchDef* switchDef = static_cast<MR::AttribDataSwitchDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));
			MR::AttribDataFloatArray* childNodeWeights = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

			for (uint32_t i = 0; i < childNodeCount; i++)
			{
				sprintf_s(paramName, "SourceWeight_%d", i);
				nodeDataBlock->writeFloat(childNodeWeights->m_values[i], paramName);
			}

			bool wrapWeights = (childNodeWeights->m_numValues == (childNodeCount + 1));
			nodeDataBlock->writeBool(wrapWeights, "WrapWeights");
			nodeDataBlock->writeUInt(switchDef->m_evalMode, "EvaluationMethod");
			nodeDataBlock->writeUInt(switchDef->m_inputSelectionMethod, "InputSelectionMethod");

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeSequenceDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_SEQUENCE);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int childNodeCount = nodeDef->getNumChildNodes();

			nodeDataBlock->writeInt(childNodeCount, "ConnectedPinCount");

			CHAR paramName[256];
			for (uint32_t i = 0; i < childNodeCount; i++)
			{
				sprintf_s(paramName, "ConnectedNodeID_%d", i);
				nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(i), paramName);
			}

			MR::AttribDataBool* loopable = static_cast<MR::AttribDataBool*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CP_BOOL));
			nodeDataBlock->writeBool(loopable->m_value, "Loop");

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeScaleToDurationDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_SCALE_TO_DURATION);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Duration", nodeDef->getInputCPConnection(0), false);

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeApplyGlobalTimeDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_APPLY_GLOBAL_TIME);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "NodeConnectedTo");

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeApplyBindPoseDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_APPLY_BIND_POSE);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "NodeConnectedTo");

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeExtractJointInfoDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_EMIT_JOINT_CP_INFO);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "SourceNodeID");
			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeInt(numAnimSets, "NumAnimSets");

			CHAR paramName[256];
			for (uint32_t animSetIndex = 0; animSetIndex < numAnimSets; animSetIndex++)
			{
				MR::AttribDataUInt* jointIndexAttrib = static_cast<MR::AttribDataUInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CP_UINT, animSetIndex));
			
				sprintf_s(paramName, "JointIndex_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(jointIndexAttrib->m_value, paramName);

				MR::AttribDataInt* angleTypeAttrib = static_cast<MR::AttribDataInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CP_INT, animSetIndex));
				nodeDataBlock->writeInt(angleTypeAttrib->m_value, "AngleType");

				MR::AttribDataBool* mesaureUintAttrib = static_cast<MR::AttribDataBool*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CP_BOOL, animSetIndex));
				nodeDataBlock->writeBool(mesaureUintAttrib->m_value, "MeasureUnit");
			}

			nodeDataBlock->writeBool(getOutputSpace(nodeDef), "OutputSpace");

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeClosestAnimDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CLOSEST_ANIM);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());
			
			const int numSourceNodes = nodeDef->getNumChildNodes();

			nodeDataBlock->writeUInt(numSourceNodes, "SourceNodeCount");

			CHAR paramName[256];
			for (uint32_t i = 0; i < numSourceNodes; i++)
			{
				sprintf_s(paramName, "Source%dNodeID", i);
				nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(i), paramName);
			}

			NodeUtils::writeInputCPConnection(nodeDataBlock, "DeadBlendWeight", nodeDef->getInputCPConnection(0), false);

			MR::AttribDataClosestAnimDef* setupAttrib = static_cast<MR::AttribDataClosestAnimDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeBool(setupAttrib->m_precomputeSourcesOffline, "PrecomputeSourcesOffline");
			nodeDataBlock->writeBool(setupAttrib->m_useVelocity, "UseVelocity");
			nodeDataBlock->writeFloat(setupAttrib->m_influenceBetweenPosAndOrient, "InfluenceBetweenPositionAndOrientation");
			nodeDataBlock->writeBool(setupAttrib->m_useRootRotationBlending, "UseRootRotationBlending");
			nodeDataBlock->writeFloat(setupAttrib->m_fractionThroughSource, "BlendDuration");

			float matchTolerance = acosf(2 * setupAttrib->m_maxRootRotationAngle);
			nodeDataBlock->writeFloat(matchTolerance, "MatchTolerance");

			nodeDataBlock->writeUInt(NodeUtils::getAxisIndex(setupAttrib->m_rootRotationAxis));

			const int numAnimSets = netDef->getNumAnimSets();

			for (uint32_t animSetIndex = 0; animSetIndex < numAnimSets; animSetIndex++)
			{
				MR::AttribDataClosestAnimDefAnimSet* closestAnimDefAnimSetAttrib = static_cast<MR::AttribDataClosestAnimDefAnimSet*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET));
				
				sprintf_s(paramName, "NumAlphaValuesSet_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(closestAnimDefAnimSetAttrib->m_numEntries, paramName);

				for (uint32_t rigChanIndex = 0; rigChanIndex < closestAnimDefAnimSetAttrib->m_numEntries; rigChanIndex++)
				{
					sprintf_s(paramName, "Alpha_%d_Set_%d", rigChanIndex + 1, animSetIndex + 1);
					nodeDataBlock->writeFloat(closestAnimDefAnimSetAttrib->m_weights[rigChanIndex], paramName);
				}
			}

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeRetargetDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_RETARGET);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numSourceNodes = nodeDef->getNumChildNodes();

			nodeDataBlock->writeUInt(nodeDef->getChildNodeID(0), "SourceNodeID");

			//We're not decompiling asset manager temp networks
			nodeDataBlock->writeBool(false, "AssetManagerExport");

			const int numAnimSets = netDef->getNumAnimSets();

			CHAR paramName[256];
			for (uint32_t animSetIndex = 0; animSetIndex < numAnimSets; animSetIndex++)
			{
				MR::AttribDataUInt* inputAnimSetIndexAttrib = static_cast<MR::AttribDataUInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_ACTIVE_ANIM_SET_INDEX, animSetIndex));

				sprintf_s(paramName, "InputAnimSetIndex_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(inputAnimSetIndexAttrib->m_value, paramName);
			}

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeScaleCharacterDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_SCALE_CHARACTER);

			ME::NodeExportXML* nodeExportXML = NodeDecompilerBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numSourceNodes = nodeDef->getNumChildNodes();

			nodeDataBlock->writeUInt(nodeDef->getChildNodeID(0), "SourceNodeID");

			//TODO: Figure out a way to recover the message ID
			nodeDataBlock->writeUInt(-1, "ScaleMessageID");

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeDecompiler::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			NodeDecompilerBase* nodeExport = nullptr;

			switch (nodeDef->getNodeTypeID())
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
}