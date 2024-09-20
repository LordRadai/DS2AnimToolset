#include "BlendNodeExport.h"
#include "NodeExport.h"
#include "extern.h"
#include "RCore.h"
#include "NodeExportHelpers.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "assetProcessor/include/assetProcessor/BlendNodeBuilderUtils.h"

namespace MorphemeExport
{
	namespace NodeExport
	{
		void writeTimeStretchModeFlags(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
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

			NodeExportHelpers::writeSyncEventFlagsFromAttribData(attribDataBlock, durationEventMatchingOpAttrib);

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

		void writeBlend2EventBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
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
			else if (taskQueueFn == MR::queuePassThroughChild0 || taskQueueFn == MR::queuePassThroughChild1)
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

		bool isBlendNodeWrapWeights(MR::NodeDef* nodeDef)
		{
			int numSourceNodes = nodeDef->getNumChildNodes();

			MR::AttribDataFloatArray* weights = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

			if (weights->m_numValues == numSourceNodes)
				return false;

			return true;
		}

		ME::NodeExportXML* exportBlend2Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_BLEND_2)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_BLEND_2, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "Source0NodeID");
			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(1), "Source1NodeID");
			nodeDataBlock->writeNetworkNodeId(nodeDef->getInputCPConnectionSourceNodeID(0), "Weight");
			nodeDataBlock->writeNetworkNodeId(nodeDef->getInputCPConnectionSourceNodeID(1), "EventBlendingWeight");

			MR::AttribDataFloatArray* weights = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

			assert(weights->m_numValues == 2);

			nodeDataBlock->readFloat(weights->m_values[0], "BlendWeight_0");
			nodeDataBlock->readFloat(weights->m_values[1], "BlendWeight_1");

			writePassThroughMode(nodeDef, nodeDataBlock);
			writeBlend2EventBlendMode(nodeDef, nodeDataBlock);
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

				nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(i), paramName);
			}

			nodeDataBlock->writeNetworkNodeId(nodeDef->getInputCPConnectionSourceNodeID(0), "Weight");

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

			writeTimeStretchModeFlags(nodeDef, nodeDataBlock);
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

			MR::AttribDataFloatArray* childWeights = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

			nodeDataBlock->writeFloat(childWeights->m_values[0], "BlendWeight_0");
			nodeDataBlock->writeFloat(childWeights->m_values[1], "BlendWeight_1");

			writePassThroughMode(nodeDef, nodeDataBlock);
			writeTimeStretchModeFlags(nodeDef, nodeDataBlock);
			writeFeatherBlendModeFlags(nodeDef, nodeDataBlock);
			writeBlend2EventBlendMode(nodeDef, nodeDataBlock);
			writeSlerpTrajPos(nodeDef, nodeDataBlock);

			MR::AttribDataBlendFlags* blendFlags = static_cast<MR::AttribDataBlendFlags*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_BLEND_FLAGS));
			nodeDataBlock->writeBool(blendFlags->m_alwaysCombineSampledEvents, "AlwaysCombineSampledEvents");

			nodeDataBlock->writeInt(numAnimSets, "NumAnimSets");

			char paramNumAlphas[256];
			char paramAlpha[256];
			for (short setIndex = 0; setIndex < numAnimSets; setIndex++)
			{
				MR::AttribDataFeatherBlend2ChannelAlphas* channelAlphas = static_cast<MR::AttribDataFeatherBlend2ChannelAlphas*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_BONE_WEIGHTS, setIndex));

				int numAlphaValues = channelAlphas->m_numChannelAlphas;

				sprintf_s(paramNumAlphas, "ChannelAlphasSet%dCount", setIndex);
				nodeDataBlock->writeInt(numAlphaValues, paramNumAlphas);

				for (int j = 0; j < numAlphaValues; j++)
				{
					sprintf_s(paramAlpha, "ChannelAlphasSet%d_Value%d", setIndex, j);
					nodeDataBlock->writeFloat(channelAlphas->m_channelAlphas[j], paramAlpha);
				}
			}

			return nodeExportXML;
		}
	}
}