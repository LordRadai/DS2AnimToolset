#include "Node.h"
#include "morpheme/Nodes/mrNodeMirrorTransforms.h"
#include "../TransitCondition/TransitCondition.h"
#include "../../MorphemeUtils/MorphemeUtils.h"
#include "extern.h"
#include "RCore.h"
#include "morpheme/Nodes/mrNodeFreeze.h"

namespace MD
{
	namespace Node
	{
		void exportNodeTransitions(MR::NodeDef* nodeDef, ME::NodeExportXML* nodeExport)
		{
			MR::NetworkDef* netDef = nodeDef->getOwningNetworkDef();

			MR::NodeDef* parentSM = MorphemeUtils::getParentStateMachineNode(nodeDef);

			if (parentSM == nullptr)
				return;

			MR::AttribDataStateMachineDef* stateMachineDef = static_cast<MR::AttribDataStateMachineDef*>(parentSM->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			MR::StateDef* targetStateDef = MorphemeUtils::getTargetNodeStateDef(nodeDef->getNodeID(), stateMachineDef);

			if (targetStateDef != nullptr)
			{
				int numConditions = targetStateDef->getNumExitConditions();

				for (size_t i = 0; i < numConditions; i++)
				{
					int conditionIndex = targetStateDef->getExitConditionStateMachineIndex(i);
					MR::TransitConditionDef* transitCondDef = stateMachineDef->getConditionDef(conditionIndex);

					MR::StateDef* referencingTransit = MorphemeUtils::getStateDefReferencingCondition(conditionIndex, stateMachineDef);

					MD::TransitCondition::exportTransitCondition(nodeExport, transitCondDef);
				}

				for (int i = 0; i < targetStateDef->getNumExitTransitionStates(); i++)
				{
					MR::StateDef* transitStateDef = stateMachineDef->getStateDef(targetStateDef->getExitTransitionStateID(i));

					assert(transitStateDef->getNumEntryConditions() != 0);

					std::vector<unsigned int> indices;
					indices.reserve(numConditions);
					for (int j = 0; j < transitStateDef->getNumEntryConditions(); j++)
					{
						int conditionID = transitStateDef->getEntryConditionStateMachineIndex(j);
						indices.push_back(MorphemeUtils::getConditionIndexByID(targetStateDef, conditionID));
					}

					int targetNodeID = transitStateDef->getNodeID();
					MD::TransitCondition::exportTransitConditionSet(nodeExport, targetNodeID, indices);
				}
			}
		}

		void setNodeName(ME::NodeExportXML* node)
		{

		}

		bool isPassThroughTransformsOnce(MR::NodeDef* nodeDef)
		{
			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
			const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

			g_appLog->debugMessage(MsgLevel_Debug, "\tATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER fn = %s\n", fnName);

			if (taskQueueFn == MR::nodeFreezePassThroughLastTrajectoryDeltaAndTransformsOnce)
				return true;
			else if (taskQueueFn == MR::nodeFreezePassThroughLastTrajectoryDeltaAndTransforms)
				return false;
			else
				g_appLog->panicMessage("Unexpected task queing function %s\n", fnName);

			return false;
		}

		ME::NodeExportXML* exportNodeCore(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			ME::NodeExportXML* nodeExportXML = static_cast<ME::NodeExportXML*>(netDefExport->createNode(nodeDef->getNodeID(), nodeDef->getNodeTypeID(), nodeDef->getParentNodeID(), false, netDef->getNodeNameFromNodeID(nodeDef->getNodeID())));
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

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
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_NETWORK);

			return exportNodeCore(netDefExport, netDef, nodeDef);
		}

		ME::NodeExportXML* exportAnimSyncEventsNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_ANIM_EVENTS);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			int numAnimSets = netDef->getNumAnimSets();

			MR::AttribDataBool* isLoop = static_cast<MR::AttribDataBool*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_LOOP));

			for (short setIndex = 0; setIndex < netDef->getNumAnimSets(); setIndex++)
			{
				MR::AttribDataSourceAnim* sourceAnim = static_cast<MR::AttribDataSourceAnim*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM, setIndex));

				if (sourceAnim)
				{
					if (setIndex == 0)
					{
						nodeDataBlock->writeAnimationId(sourceAnim->m_animAssetID, "AnimIndex");
						nodeDataBlock->writeBool(false, "GenerateAnimationDeltas");
						nodeDataBlock->writeBool(isLoop->m_value, "Loop");
						nodeDataBlock->writeBool(sourceAnim->m_playBackwards, "PlayBackwards");
						nodeDataBlock->writeBool(false, "PreComputeSyncEventTracks");
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
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_MIRROR_TRANSFORMS_ID);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
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

			bool eventPassThrough = false;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TIME_POS);
			const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

			if (taskQueueFn == MR::nodeMirrorQueueTimePos)
				eventPassThrough = true;

			nodeDataBlock->writeBool(eventPassThrough, "EventPassThrough");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportFilterTransformsNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_FILTER_TRANSFORMS);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "NodeConnectedTo");
			nodeDataBlock->writeInt(numAnimSets, "NumAnimSets");

			for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; animSetIndex++)
			{
				CHAR paramName[256];

				sprintf_s(paramName, "FilterIdCount_%d", animSetIndex + 1); // We add one to the index as LUA arrays start at 1 and the manifest was written out using LUA array indices

				MR::AttribDataUIntArray* numBoneFilterIDs = static_cast<MR::AttribDataUIntArray*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_BONE_IDS, animSetIndex));

				nodeDataBlock->writeInt(numBoneFilterIDs->m_numValues);

				for (uint32_t k = 0; k < numBoneFilterIDs->m_numValues; k++)
				{
					sprintf_s(paramName, "Id_%d_%d", animSetIndex + 1, k + 1);
					nodeDataBlock->writeUInt(numBoneFilterIDs->m_values[k], paramName);
				}
			}

			return nodeExportXML;
		}

		ME::NodeExportXML* exportSingleFrameNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_SINGLEFRAME);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(1)->m_sourceNodeID, nodeDef->getInputCPConnection(1)->m_sourcePinIndex, "Control");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportFreezeNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_FREEZE);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			int numAnimSets = netDef->getNumAnimSets();

			nodeDataBlock->writeBool(isPassThroughTransformsOnce(nodeDef), "passThroughTransformsOnce");

			return nodeExportXML;
		}
	}
}