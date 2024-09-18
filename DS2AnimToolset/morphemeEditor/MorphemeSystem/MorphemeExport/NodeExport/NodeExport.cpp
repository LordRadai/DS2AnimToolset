#include "NodeExport.h"
#include "../TransitConditionExport/TransitConditionExport.h"
#include "../../MRUtils/MRUtils.h"
#include "extern.h"
#include "RCore.h"

using namespace MR;

namespace MorphemeExport
{
	namespace NodeExport
	{
		void exportNodeTransitions(MR::NodeDef* nodeDef, ME::NodeExportXML* nodeExport)
		{
			MR::NetworkDef* netDef = nodeDef->getOwningNetworkDef();

			MR::NodeDef* parentSM = MRUtils::getParentStateMachineNode(nodeDef);

			if (parentSM == nullptr)
				return;

			MR::AttribDataStateMachineDef* stateMachineDef = static_cast<MR::AttribDataStateMachineDef*>(parentSM->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			MR::StateDef* targetStateDef = MRUtils::getTargetNodeStateDef(nodeDef->getNodeID(), stateMachineDef);

			if (targetStateDef != nullptr)
			{
				int numConditions = targetStateDef->getNumExitConditions();

				for (size_t i = 0; i < numConditions; i++)
				{
					int conditionIndex = targetStateDef->getExitConditionStateMachineIndex(i);
					MR::TransitConditionDef* transitCondDef = stateMachineDef->getConditionDef(conditionIndex);

					MorphemeExport::TransitConditionExport::exportTransitCondition(nodeExport, transitCondDef);
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
						indices.push_back(MRUtils::getConditionIndexByID(targetStateDef, conditionID));
					}

					int targetNodeID = stateMachineDef->getStateDef(transitStateDef->getTransitDestinationStateID())->getNodeID();
					MorphemeExport::TransitConditionExport::exportTransitConditionSet(nodeExport, targetNodeID, indices);
				}
			}
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

				MR::AttribDataInt* eventOffset = static_cast<MR::AttribDataInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_SYNC_EVENT_OFFSET, animSetIndex));
				nodeDataBlock->writeUInt(eventOffset->m_value, "EventOffset");
			}

			bool eventPassThrough = false;

			MR::QueueAttrTaskFn taskQueueFn = nodeDef->getTaskQueueingFn(MR::ATTRIB_SEMANTIC_TIME_POS);
			const char* fnName = MR::Manager::getInstance().getTaskQueuingFnName(taskQueueFn);

			if (taskQueueFn == MR::nodeMirrorQueueTimePos)
				eventPassThrough = true;

			nodeDataBlock->writeBool(eventPassThrough, "EventPassThrough");

			return nodeExportXML;
		}
	}
}