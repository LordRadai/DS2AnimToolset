#include "BlendNode.h"
#include "Node.h"
#include "NodeUtils.h"
#include "extern.h"
#include "RCore.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFeatherBlend2.h"
#include "morpheme/Nodes/mrNodeFeatherBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeSubtractiveBlend.h"
#include "morpheme/Nodes/mrNodeSubtractiveBlendSyncEvents.h"
#include "morpheme/Nodes/mrNodeBlend2x2.h"
#include "morpheme/Nodes/mrNodeBlend2x2SyncEvents.h"
#include "assetProcessor/include/assetProcessor/BlendNodeBuilderUtils.h"

namespace MD
{
	namespace Node
	{
		void BlendNodeExportBase::writeTimeStretchMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			int timeStretchMode = AP::kNodeTimeStretchNone;

			MR::AttribDataUInt* startSyncEventIndex = static_cast<MR::AttribDataUInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX));
			MR::AttribDataUInt* durationEventMatchingOpAttrib = static_cast<MR::AttribDataUInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP));
			MR::AttribDataBool* loop = static_cast<MR::AttribDataBool*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_LOOP));

			MR::QueueAttrTaskFn syncEventTrackFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS);

			if (syncEventTrackFn == MR::nodeBlend2SyncEventsQueueTimePos)
				timeStretchMode = AP::kNodeTimeStretchMatchEvents;

			attribDataBlock->writeInt(timeStretchMode, "TimeStretchMode");

			if (loop)
				attribDataBlock->writeBool(loop->m_value, "Loop");

			NodeUtils::writeSyncEventFlagsFromAttribData(attribDataBlock, durationEventMatchingOpAttrib);

			if (startSyncEventIndex)
				attribDataBlock->writeInt(startSyncEventIndex->m_value, "StartEventIndex");
		}

		void BlendNodeExportBase::writeEventBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			AP::NodeSampledEventBlendModes eventBlendMode = AP::kSampledEventBlendModeInvalid;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
			const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

			if ((taskQueueFn == MR::nodeBlend2SyncEventsQueueSampledEventsBuffers) || (taskQueueFn == MR::nodeBlend2QueueSampledEventsBuffers))
				eventBlendMode = AP::kMergeSampledEvents;
			else if ((taskQueueFn == MR::nodeBlend2SyncEventsQueueAddSampledEventsBuffers) || (taskQueueFn == MR::nodeBlend2QueueAddSampledEventsBuffers))
				eventBlendMode = AP::kAddSampledEvents;

			attribDataBlock->writeInt(eventBlendMode, "EventsBlendMode");
		}

		void BlendNodeExportBase::writeBlendFlags(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			MR::AttribDataBlendFlags* blendFlags = static_cast<MR::AttribDataBlendFlags*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_BLEND_FLAGS));
			attribDataBlock->writeBool(blendFlags->m_alwaysCombineSampledEvents, "AlwaysCombineSampledEvents");
		}

		void NodeExportBlend2::writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			bool slerpTrajPos = false;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

			if ((taskQueueFn == MR::nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj) ||
				(taskQueueFn == MR::nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj) ||
				(taskQueueFn == MR::nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj) ||
				(taskQueueFn == MR::nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj))
				slerpTrajPos = true;

			attribDataBlock->writeBool(slerpTrajPos, "SphericallyInterpolateTrajectoryPosition");
		}

		void NodeExportBlend2x2::writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			bool slerpTrajPos = false;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

			if ((taskQueueFn == MR::nodeBlend2x2QueueTrajectoryDeltaAndTransformsInterpPosInterpAttSlerpTraj))
				slerpTrajPos = true;

			attribDataBlock->writeBool(slerpTrajPos, "SphericallyInterpolateTrajectoryPosition");
		}

		void NodeExportBlendN::writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			bool slerpTrajPos = false;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

			if ((taskQueueFn == MR::nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj))
				slerpTrajPos = true;

			attribDataBlock->writeBool(slerpTrajPos, "SphericallyInterpolateTrajectoryPosition");
		}

		void NodeExportBlendNxM::writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			bool slerpTrajPos = false;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

			if ((taskQueueFn == MR::nodeBlend2x2QueueTrajectoryDeltaAndTransformsInterpPosInterpAttSlerpTraj))
				slerpTrajPos = true;

			attribDataBlock->writeBool(slerpTrajPos, "SphericallyInterpolateTrajectoryPosition");
		}

		void NodeExportFeatherBlend::writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			bool slerpTrajPos = false;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

			if ((taskQueueFn == MR::nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj) ||
				(taskQueueFn == MR::nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj) ||
				(taskQueueFn == MR::nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj) ||
				(taskQueueFn == MR::nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj))
				slerpTrajPos = true;

			attribDataBlock->writeBool(slerpTrajPos, "SphericallyInterpolateTrajectoryPosition");
		}

		void NodeExportSubtractiveBlend::writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			bool slerpTrajPos = false;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
			if ((taskQueueFn == MR::nodeSubtractiveBlendQueueTrajectoryDeltaAndTransformsSubtractAttSubtractPosSlerpTraj))
				slerpTrajPos = true;

			attribDataBlock->writeBool(slerpTrajPos, "SphericallyInterpolateTrajectoryPosition");
		}

		void BlendNodeExportBase::writePassThroughMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			if (nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_FILTER)
			{
				uint16_t passThroughChildIndex = nodeDef->getPassThroughChildIndex();

				switch (passThroughChildIndex)
				{
				case 0:
					attribDataBlock->writeInt(AP::kNodePassThroughSource0, "PassThroughMode");
					break;
				case 1:
					attribDataBlock->writeInt(AP::kNodePassThroughSource1, "PassThroughMode");
					break;
				}

				return;
			}

			attribDataBlock->writeInt(AP::kNodePassThroughNone, "PassThroughMode");
		}

		bool BlendNodeExportBase::isBlendNodeWrapWeights(MR::NodeDef* nodeDef)
		{
			int numSourceNodes = nodeDef->getNumChildNodes();

			MR::AttribDataFloatArray* weights = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

			if (weights->m_numValues == numSourceNodes)
				return false;

			return true;
		}

		void NodeExportBlend2::writeBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			AP::NodeBlendModes blendMode = AP::kNodeBlendInvalid;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
			const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

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

		ME::NodeExportXML* NodeExportBlend2::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_BLEND_2);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "Source0NodeID");
			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(1), "Source1NodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Weight", nodeDef->getInputCPConnection(0), true);

			MR::AttribDataFloatArray* weights = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

			assert(weights->m_numValues == 2);

			nodeDataBlock->writeFloat(weights->m_values[0], "BlendWeight_0");
			nodeDataBlock->writeFloat(weights->m_values[1], "BlendWeight_1");

			writePassThroughMode(nodeDef, nodeDataBlock);
			writeEventBlendMode(nodeDef, nodeDataBlock);
			writeSlerpTrajPos(nodeDef, nodeDataBlock);
			writeBlendMode(nodeDef, nodeDataBlock);
			writeTimeStretchMode(nodeDef, nodeDataBlock);
			writeBlendFlags(nodeDef, nodeDataBlock);

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeExportBlend2x2::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_BLEND_2X2);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "Source0NodeID");
			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(1), "Source1NodeID");
			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(2), "Source2NodeID");
			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(3), "Source3NodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "WeightX", nodeDef->getInputCPConnection(0), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "WeightY", nodeDef->getInputCPConnection(1), true);

			MR::AttribDataBlendNxMDef* weights = static_cast<MR::AttribDataBlendNxMDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

			nodeDataBlock->writeFloat(weights->m_weightsX[0], "BlendWeightX_0");
			nodeDataBlock->writeFloat(weights->m_weightsX[1], "BlendWeightX_1");
			nodeDataBlock->writeFloat(weights->m_weightsY[0], "BlendWeightY_0");
			nodeDataBlock->writeFloat(weights->m_weightsY[1], "BlendWeightY_1");

			writeSlerpTrajPos(nodeDef, nodeDataBlock);
			writeTimeStretchMode(nodeDef, nodeDataBlock);
			writeBlendFlags(nodeDef, nodeDataBlock);

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeExportBlendN::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_BLEND_N);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int sourceNodeCount = nodeDef->getNumChildNodes();

			nodeDataBlock->writeInt(sourceNodeCount, "SourceNodeCount");

			CHAR paramName[256];
			for (int i = 0; i < sourceNodeCount; i++)
			{
				sprintf_s(paramName, "Source%dNodeID", i);

				nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(i), paramName);
			}

			NodeUtils::writeInputCPConnection(nodeDataBlock, "Weight", nodeDef->getInputCPConnection(0), true);

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
			writeBlendFlags(nodeDef, nodeDataBlock);

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeExportBlendNxM::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_BLEND_NXM);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int sourceNodeCount = nodeDef->getNumChildNodes();

			MR::AttribDataBlendNxMDef* blendNxMAttrib = static_cast<MR::AttribDataBlendNxMDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

			nodeDataBlock->writeInt(blendNxMAttrib->m_numWeightsX, "NodeCountX");
			nodeDataBlock->writeInt(blendNxMAttrib->m_numWeightsY, "NodeCountY");

			assert(sourceNodeCount == blendNxMAttrib->m_numWeightsX * blendNxMAttrib->m_numWeightsY);

			CHAR paramName[256];
			for (int i = 0; i < sourceNodeCount; i++)
			{
				sprintf_s(paramName, "Source%dNodeID", i);

				nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(i), paramName);
			}

			NodeUtils::writeInputCPConnection(nodeDataBlock, "WeightX", nodeDef->getInputCPConnection(0), true);
			NodeUtils::writeInputCPConnection(nodeDataBlock, "WeightY", nodeDef->getInputCPConnection(1), true);

			nodeDataBlock->writeBool(blendNxMAttrib->m_wrapWeightsX, "WrapWeightsX");
			nodeDataBlock->writeBool(blendNxMAttrib->m_wrapWeightsY, "WrapWeightsY");

			for (uint32_t i = 0; i < blendNxMAttrib->m_numWeightsX; i++)
			{
				sprintf_s(paramName, "BlendWeightX_%d", i);
				nodeDataBlock->writeFloat(blendNxMAttrib->m_weightsX[i], paramName);
			}

			if (blendNxMAttrib->m_wrapWeightsX)
				nodeDataBlock->writeFloat(blendNxMAttrib->m_weightsX[blendNxMAttrib->m_numWeightsX], "WrapWeightX");

			for (uint32_t i = 0; i < blendNxMAttrib->m_numWeightsY; i++)
			{
				sprintf_s(paramName, "BlendWeightY_%d", i);
				nodeDataBlock->writeFloat(blendNxMAttrib->m_weightsY[i], paramName);
			}

			if (blendNxMAttrib->m_wrapWeightsX)
				nodeDataBlock->writeFloat(blendNxMAttrib->m_weightsY[blendNxMAttrib->m_numWeightsY], "WrapWeightY");

			writeTimeStretchMode(nodeDef, nodeDataBlock);
			writeSlerpTrajPos(nodeDef, nodeDataBlock);
			writeBlendFlags(nodeDef, nodeDataBlock);

			return nodeExportXML;
		}

		ME::NodeExportXML* NodeExportBlendAll::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_BLEND_ALL);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int sourceNodeCount = nodeDef->getNumChildNodes();
			const int inputCPCount = nodeDef->getNumInputCPConnections();

			if (inputCPCount != sourceNodeCount)
				g_appLog->panicMessage("Mismatch between inputCPCount and sourceNodeCount (nodeID=%d)\n", nodeDef->getNodeID());

			nodeDataBlock->writeInt(sourceNodeCount, "SourceNodeCount");

			CHAR paramName[256];
			for (int i = 0; i < sourceNodeCount; i++)
			{
				sprintf_s(paramName, "Source%dNodeID", i);
				nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(i), paramName);
			}

			for (int i = 0; i < inputCPCount; i++)
			{
				sprintf_s(paramName, "Weight%d", i);
				NodeUtils::writeInputCPConnection(nodeDataBlock, paramName, nodeDef->getInputCPConnection(i), true);
			}

			writeTimeStretchMode(nodeDef, nodeDataBlock);
			writeSlerpTrajPos(nodeDef, nodeDataBlock);

			return nodeExportXML;
		}

		void NodeExportFeatherBlend::writeBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock)
		{
			AP::NodeBlendModes blendMode = AP::kNodeBlendInvalid;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
			const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

			if (taskQueueFn == MR::nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttAddPos)
				blendMode = AP::kAddQuatAddPos;
			else if (taskQueueFn == MR::nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttInterpPos)
				blendMode = AP::kAddQuatInterpPos;
			else if (taskQueueFn == MR::nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttAddPos)
				blendMode = AP::kInterpQuatAddPos;
			else if (taskQueueFn == MR::nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttInterpPos)
				blendMode = AP::kInterpQuatInterpPos;
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

		ME::NodeExportXML* NodeExportFeatherBlend::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_FEATHER_BLEND_2);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "Source0NodeID");
			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(1), "Source1NodeID");
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Weight", nodeDef->getInputCPConnection(0), true);

			MR::AttribDataFloatArray* childWeights = static_cast<MR::AttribDataFloatArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS));

			nodeDataBlock->writeFloat(childWeights->m_values[0], "BlendWeight_0");
			nodeDataBlock->writeFloat(childWeights->m_values[1], "BlendWeight_1");

			writePassThroughMode(nodeDef, nodeDataBlock);
			writeTimeStretchMode(nodeDef, nodeDataBlock);
			writeBlendMode(nodeDef, nodeDataBlock);
			writeEventBlendMode(nodeDef, nodeDataBlock);
			writeSlerpTrajPos(nodeDef, nodeDataBlock);
			writeBlendFlags(nodeDef, nodeDataBlock);

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

		ME::NodeExportXML* NodeExportSubtractiveBlend::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_SUBTRACTIVE_BLEND);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			const int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "Source0NodeID");
			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(1), "Source1NodeID");

			writeTimeStretchMode(nodeDef, nodeDataBlock);
			writeSlerpTrajPos(nodeDef, nodeDataBlock);
			writePassThroughMode(nodeDef, nodeDataBlock);
			writeEventBlendMode(nodeDef, nodeDataBlock);

			return nodeExportXML;
		}
	}
}