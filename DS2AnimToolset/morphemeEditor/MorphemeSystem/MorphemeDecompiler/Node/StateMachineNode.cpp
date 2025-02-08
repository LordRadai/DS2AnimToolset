#include "StateMachineNode.h"
#include "../TransitCondition/TransitCondition.h"
#include "../../MorphemeUtils/MorphemeUtils.h"
#include "Node.h"
#include "NodeUtils.h"
#include "extern.h"
#include "RCore.h"

namespace MD
{
	namespace Node
	{
		void NodeExportStateMachine::writeActiveStateTransitions(MR::AttribDataStateMachineDef* stateMachineDef, ME::NodeExportXML* nodeExport)
		{
			MR::StateDef* globalStateDef = stateMachineDef->getGlobalStateDef();

			int numConditions = globalStateDef->getNumExitConditions();

			for (int i = 0; i < numConditions; i++)
			{
				int conditionIndex = globalStateDef->getExitConditionStateMachineIndex(i);
				MR::TransitConditionDef* transitCondDef = stateMachineDef->getConditionDef(conditionIndex);

				MD::TransitCondition::exportTransitCommonCondition(nodeExport, transitCondDef);
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
					indices.push_back(MorphemeUtils::getConditionIndexByID(globalStateDef, conditionID));
				}

				//This is supposed to be a global state. If this condition is not met then I'm doing something wrong
				assert(transitStateDef->getTransitSourceStateID() == MR::INVALID_NODE_ID);

				int targetNodeID = transitStateDef->getNodeID();
				MD::TransitCondition::exportTransitCommonConditionSet(nodeExport, targetNodeID, indices);
			}
		}

		void NodeExportStateMachine::writeChildStates(MR::NodeDef* nodeDef, ME::DataBlockExportXML* nodeDataBlock)
		{
			MR::NetworkDef* netDef = nodeDef->getOwningNetworkDef();

			std::vector<MR::StateDef*> childNodes = MorphemeUtils::getStateMachineSteadyChildNodes(netDef, nodeDef);

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

			std::vector<MR::StateDef*> childTransitNodes = MorphemeUtils::getStateMachineTransitChildNodes(netDef, nodeDef);

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

		ME::NodeExportXML* NodeExportStateMachine::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_STATE_MACHINE);

			ME::NodeExportXML* nodeExportXML = NodeExportBase::exportNode(netDefExport, netDef, nodeDef, nodeName);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataStateMachineDef* stateMachineDef = static_cast<MR::AttribDataStateMachineDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			writeChildStates(nodeDef, nodeDataBlock);

			int defaultStateID = stateMachineDef->getStateDef(stateMachineDef->getDefaultStartingStateID())->getNodeID();
			nodeDataBlock->writeNetworkNodeId(defaultStateID, "DefaultNodeID");

			writeActiveStateTransitions(stateMachineDef, nodeExportXML);

			return nodeExportXML;
		}
	}
}