#include "TransitNode.h"
#include "../../MorphemeUtils/MorphemeUtils.h"
#include "Node.h"
#include "NodeUtils.h"
#include "extern.h"
#include "RCore.h"
#include "morpheme/mrBlendOps.h"

namespace MD
{
	namespace Node
	{
		void writeDeltaTrajSrouce(MR::NodeDef* nodeDef, ME::DataBlockExportXML* dataBlockExport)
		{
			MR::QueueAttrTaskFn trajDeltaTransformTaskFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

			int deltaTrajSource = 0;

			if (trajDeltaTransformTaskFn == MR::queuePassThroughChild0IfNotPhysics)
				deltaTrajSource = 2;
			else if (trajDeltaTransformTaskFn == MR::queuePassThroughChild1IfNotPhysics)
				deltaTrajSource = 1;

			dataBlockExport->writeUInt(deltaTrajSource, "DeltaTrajSource");
		}

		ME::NodeExportXML* exportTransitNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			if ((nodeDef->getNodeTypeID() != NODE_TYPE_TRANSIT) && (nodeDef->getNodeTypeID() != NODE_TYPE_TRANSIT_PHYSICS))
				g_appLog->panicMessage("Expecting node type %d or %d (got %d)\n", NODE_TYPE_TRANSIT, NODE_TYPE_TRANSIT_PHYSICS, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			if (nodeDef->getChildNodeID(0) != MR::INVALID_NODE_ID)
				nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "SourceNodeID");

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(1), "DestNodeID");

			MR::AttribDataTransitDef* transitDef = static_cast<MR::AttribDataTransitDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeFloat(transitDef->m_duration, "DurationInTime");
			nodeDataBlock->writeFloat(transitDef->m_destinationStartFraction, "DestinationStartFraction");
			nodeDataBlock->writeFloat(transitDef->m_destinationStartSyncEvent, "DestinationStartSyncEvent");

			bool useDestinationStartFraction = false;
			bool useDestinationStartSyncEventIndex = false;
			bool useDestinationStartSyncEventFraction = false;

			switch (transitDef->m_destinationInitMethod)
			{
			case MR::AttribDataTransitDef::INIT_DEST_USE_START_DURATION_FRACTION:
				useDestinationStartFraction = true;
				break;
			case MR::AttribDataTransitDef::INIT_DEST_USE_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION:
				useDestinationStartSyncEventIndex = true;
				break;
			case MR::AttribDataTransitDef::INIT_DEST_USE_EVENT_INDEX_AND_USE_EVENT_FRACTION:
				useDestinationStartSyncEventFraction = true;
				break;
			case MR::AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION:
				break;
			case MR::AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_USE_EVENT_FRACTION:
				useDestinationStartSyncEventFraction = true;
				break;
			default:
				g_appLog->panicMessage("Invalid DestinationInitMethod %d\n", transitDef->m_destinationInitMethod);
				break;
			}

			nodeDataBlock->writeBool(useDestinationStartFraction, "UseDestinationStartFraction");
			nodeDataBlock->writeBool(useDestinationStartSyncEventIndex, "UseDestinationStartSyncEventIndex");
			nodeDataBlock->writeBool(useDestinationStartSyncEventFraction, "UseDestinationStartSyncEventFraction");
			
			bool breakoutTransit = false;
			if (nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_BREAKOUT_TRANSITION)
				breakoutTransit = true;

			nodeDataBlock->writeBool(breakoutTransit, "BreakoutTransit");

			nodeDataBlock->writeBool(transitDef->m_reversible, "isReversibleTransit");

			if (transitDef->m_reversible)
				nodeDataBlock->writeInt(transitDef->m_reverseInputCPConnection.m_sourceNodeID, "RuntimeNodeID");

			nodeDataBlock->writeBool(transitDef->m_freezeSource, "FreezeSource");
			nodeDataBlock->writeBool(transitDef->m_freezeDest, "FreezeDest");

			bool additiveBlendAtt = false;
			bool additiveBlendPos = false;
			switch (transitDef->m_blendMode)
			{
			case MR::BLEND_MODE_INTERP_ATT_INTERP_POS:
				break;
			case MR::BLEND_MODE_INTERP_ATT_ADD_POS:
				additiveBlendPos = true;
				break;
			case MR::BLEND_MODE_ADD_ATT_LEAVE_POS:
				additiveBlendAtt = true;
				break;
			case MR::BLEND_MODE_ADD_ATT_ADD_POS:
				additiveBlendAtt = true;
				additiveBlendPos = true;
				break;
			default:
				g_appLog->panicMessage("Invalid BlendMode %d\n", transitDef->m_blendMode);
				break;
			}

			nodeDataBlock->writeBool(additiveBlendAtt, "AdditiveBlendAttitude");
			nodeDataBlock->writeBool(additiveBlendPos, "AdditiveBlendPosition");

			nodeDataBlock->writeBool(transitDef->m_slerpTrajectoryPosition, "SphericallyInterpolateTrajectoryPosition");

			bool deadblendBreakoutToSource = false;
			if (nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_TRANSITION_CAN_DEAD_BLEND)
				deadblendBreakoutToSource = true;

			nodeDataBlock->writeBool(deadblendBreakoutToSource, "DeadblendBreakoutToSource");

			if (transitDef->m_nodeInitData)
			{
				int numDestSubStates = transitDef->m_nodeInitData->m_numNodeInitDatas;
				nodeDataBlock->writeInt(numDestSubStates, "DestinationSubStateCount");

				int32_t subStateNodeID;
				CHAR paramName[256];
				for (int i = 0; i < numDestSubStates; i++)
				{
					MR::StateMachineInitData* nodeInitData = static_cast<MR::StateMachineInitData*>(transitDef->m_nodeInitData->m_nodeInitDataArray[i]);

					sprintf_s(paramName, "DestinationSubStateID_%d", i);
					nodeDataBlock->writeInt(nodeInitData->getInitialSubStateID(), paramName);

					sprintf_s(paramName, "DestinationSubStateParentID_%d", i);
					nodeDataBlock->writeInt(nodeInitData->getTargetNodeID(), paramName);
				}
			}
			else
				nodeDataBlock->writeInt(0, "DestinationSubStateCount");

			MR::AttribDataDeadBlendDef* deadBlendDef = static_cast<MR::AttribDataDeadBlendDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_DEAD_BLEND_DEF));

			nodeDataBlock->writeBool(deadBlendDef->m_useDeadReckoningWhenDeadBlending, "UseDeadReckoningWhenDeadBlending");
			nodeDataBlock->writeBool(deadBlendDef->m_blendToDestinationPhysicsBones, "BlendToDestinationPhysicsBones");
			
			writeDeltaTrajSrouce(nodeDef, nodeDataBlock);

			return nodeExportXML;
		}

		ME::NodeExportXML* exportTransitSyncEventsNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			if ((nodeDef->getNodeTypeID() != NODE_TYPE_TRANSIT_SYNC_EVENTS) && (nodeDef->getNodeTypeID() != NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS))
				g_appLog->panicMessage("Expecting node type %d or %d (got %d)\n", NODE_TYPE_TRANSIT_SYNC_EVENTS, NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			if (nodeDef->getChildNodeID(0) != MR::INVALID_NODE_ID)
				nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "SourceNodeID");

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(1), "DestNodeID");

			MR::AttribDataTransitSyncEventsDef* transitDef = static_cast<MR::AttribDataTransitSyncEventsDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeFloat(transitDef->m_durationInEvents, "DurationInEvents");
			nodeDataBlock->writeUInt(transitDef->m_destEventTrackOffset, "DestEventSequenceOffset");
			nodeDataBlock->writeUInt(transitDef->m_destStartEventIndex, "DestStartEventIndex");
			nodeDataBlock->writeBool(transitDef->m_usingDestStartEventIndex, "UsingDestStartEventIndex");

			bool breakoutTransit = false;
			if (nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_BREAKOUT_TRANSITION)
				breakoutTransit = true;

			nodeDataBlock->writeBool(breakoutTransit, "BreakoutTransit");

			nodeDataBlock->writeBool(transitDef->m_reversible, "isReversibleTransit");

			if (transitDef->m_reversible)
				nodeDataBlock->writeInt(transitDef->m_reverseInputCPConnection.m_sourceNodeID, "RuntimeNodeID");

			bool additiveBlendAtt = false;
			bool additiveBlendPos = false;
			switch (transitDef->m_blendMode)
			{
			case MR::BLEND_MODE_INTERP_ATT_INTERP_POS:
				break;
			case MR::BLEND_MODE_INTERP_ATT_ADD_POS:
				additiveBlendPos = true;
				break;
			case MR::BLEND_MODE_ADD_ATT_LEAVE_POS:
				additiveBlendAtt = true;
				break;
			case MR::BLEND_MODE_ADD_ATT_ADD_POS:
				additiveBlendAtt = true;
				additiveBlendPos = true;
				break;
			default:
				g_appLog->panicMessage("Invalid BlendMode %d\n", transitDef->m_blendMode);
				break;
			}

			nodeDataBlock->writeBool(additiveBlendAtt, "AdditiveBlendAttitude");
			nodeDataBlock->writeBool(additiveBlendPos, "AdditiveBlendPosition");

			nodeDataBlock->writeBool(transitDef->m_slerpTrajectoryPosition, "SphericallyInterpolateTrajectoryPosition");

			bool deadblendBreakoutToSource = false;
			if (nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_TRANSITION_CAN_DEAD_BLEND)
				deadblendBreakoutToSource = true;

			nodeDataBlock->writeBool(deadblendBreakoutToSource, "DeadblendBreakoutToSource");


			if (transitDef->m_nodeInitData)
			{
				int numDestSubStates = transitDef->m_nodeInitData->m_numNodeInitDatas;
				nodeDataBlock->writeInt(numDestSubStates, "DestinationSubStateCount");

				int32_t subStateNodeID;
				CHAR paramName[256];
				for (int i = 0; i < numDestSubStates; i++)
				{
					MR::StateMachineInitData* nodeInitData = static_cast<MR::StateMachineInitData*>(transitDef->m_nodeInitData->m_nodeInitDataArray[i]);

					sprintf_s(paramName, "DestinationSubStateID_%d", i);
					nodeDataBlock->writeInt(nodeInitData->getTargetNodeID(), paramName);

					sprintf_s(paramName, "DestinationSubStateParentID_%d", i);
					nodeDataBlock->writeInt(nodeInitData->getInitialSubStateID(), paramName);
				}
			}
			else
				nodeDataBlock->writeInt(0, "DestinationSubStateCount");

			MR::AttribDataUInt* durationEventMatchingOpAttrib = static_cast<MR::AttribDataUInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP));
			NodeUtils::writeSyncEventFlagsFromAttribData(nodeDataBlock, durationEventMatchingOpAttrib);

			MR::AttribDataDeadBlendDef* deadBlendDef = static_cast<MR::AttribDataDeadBlendDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_DEAD_BLEND_DEF));

			nodeDataBlock->writeBool(deadBlendDef->m_useDeadReckoningWhenDeadBlending, "UseDeadReckoningWhenDeadBlending");
			nodeDataBlock->writeBool(deadBlendDef->m_blendToDestinationPhysicsBones, "BlendToDestinationPhysicsBones");

			writeDeltaTrajSrouce(nodeDef, nodeDataBlock);

			return nodeExportXML;
		}
	}
}