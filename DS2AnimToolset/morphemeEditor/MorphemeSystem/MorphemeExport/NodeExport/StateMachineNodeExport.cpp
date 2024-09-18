#include "StateMachineNodeExport.h"
#include "../TransitConditionExport/TransitConditionExport.h"
#include "../../MRUtils/MRUtils.h"
#include "NodeExport.h"
#include "extern.h"
#include "RCore.h"

namespace MorphemeExport
{
	namespace NodeExport
	{
		void writeActiveStateTransitions(MR::AttribDataStateMachineDef* stateMachineDef, ME::NodeExportXML* nodeExport)
		{
			MR::StateDef* globalStateDef = stateMachineDef->getGlobalStateDef();

			int numConditions = globalStateDef->getNumExitConditions();

			for (size_t i = 0; i < numConditions; i++)
			{
				int conditionIndex = globalStateDef->getExitConditionStateMachineIndex(i);
				MR::TransitConditionDef* transitCondDef = stateMachineDef->getConditionDef(conditionIndex);

				MorphemeExport::TransitConditionExport::exportTransitCommonCondition(nodeExport, transitCondDef);
			}

			for (int i = 0; i < globalStateDef->getNumExitTransitionStates(); i++)
			{
				MR::StateDef* transitStateDef = stateMachineDef->getStateDef(globalStateDef->getExitTransitionStateID(i));

				assert(transitStateDef->getNumEntryConditions() != 0);

				std::vector<unsigned int> indices;
				indices.reserve(numConditions);
				for (int j = 0; j < transitStateDef->getNumEntryConditions(); j++)
				{
					int conditionID = transitStateDef->getEntryConditionStateMachineIndex(j);
					indices.push_back(MRUtils::getConditionIndexByID(globalStateDef, conditionID));
				}

				//This is supposed to be a global state. If this condition is not met then I'm doing something wrong
				assert(transitStateDef->getTransitSourceStateID() == MR::INVALID_NODE_ID);

				int targetNodeID = transitStateDef->getNodeID();
				MorphemeExport::TransitConditionExport::exportTransitCommonConditionSet(nodeExport, targetNodeID, indices);
			}
		}

		std::vector<MR::StateDef*> getStateMachineNodeChildNodes(MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			std::vector<MR::StateDef*> childStates;

			MR::AttribDataStateMachineDef* attribData = static_cast<MR::AttribDataStateMachineDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			if (attribData == nullptr)
				return childStates;

			int childStateCount = nodeDef->getNumChildNodes();

			for (size_t i = 0; i < childStateCount; i++)
			{
				MR::StateDef* stateDef = attribData->getStateDef(i);

				int nodeID = stateDef->getNodeID();

				MR::NodeDef* def = netDef->getNodeDef(nodeID);

				int nodeType = def->getNodeTypeID();

				if ((nodeType != NODE_TYPE_TRANSIT) && (nodeType != NODE_TYPE_TRANSIT_PHYSICS) && (nodeType != NODE_TYPE_TRANSIT_SYNC_EVENTS) && (nodeType != NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS))
					childStates.push_back(stateDef);
			}

			return childStates;
		}

		std::vector<MR::StateDef*> getStateMachineNodeChildTransitNodes(MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			std::vector<MR::StateDef*> childStates;

			MR::AttribDataStateMachineDef* attribData = static_cast<MR::AttribDataStateMachineDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			if (attribData == nullptr)
				return childStates;

			int childStateCount = nodeDef->getNumChildNodes();

			for (size_t i = 0; i < childStateCount; i++)
			{
				MR::StateDef* stateDef = attribData->getStateDef(i);

				int nodeID = stateDef->getNodeID();

				MR::NodeDef* def = netDef->getNodeDef(nodeID);

				int nodeType = def->getNodeTypeID();

				if ((nodeType == NODE_TYPE_TRANSIT) || (nodeType == NODE_TYPE_TRANSIT_PHYSICS) || (nodeType == NODE_TYPE_TRANSIT_SYNC_EVENTS) || (nodeType == NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS))
					childStates.push_back(stateDef);
			}

			return childStates;
		}

		void writeStateMachineChildStates(MR::NodeDef* nodeDef, ME::DataBlockExportXML* nodeDataBlock)
		{
			MR::NetworkDef* netDef = nodeDef->getOwningNetworkDef();

			std::vector<MR::StateDef*> childNodes = getStateMachineNodeChildNodes(netDef, nodeDef);

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

			std::vector<MR::StateDef*> childTransitNodes = getStateMachineNodeChildTransitNodes(netDef, nodeDef);

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

		ME::NodeExportXML* exportStateMachineNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_STATE_MACHINE)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_STATE_MACHINE, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataStateMachineDef* stateMachineDef = static_cast<MR::AttribDataStateMachineDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			writeStateMachineChildStates(nodeDef, nodeDataBlock);

			int defaultStateID = stateMachineDef->getStateDef(stateMachineDef->getDefaultStartingStateID())->getNodeID();
			nodeDataBlock->writeUInt(defaultStateID, "DefaultNodeID");

			writeActiveStateTransitions(stateMachineDef, nodeExportXML);

			return nodeExportXML;
		}
	}
}