#include "NodeExportBase.h"
#include "../TransitCondition/TransitCondition.h"
#include "../../MorphemeUtils/MorphemeUtils.h"
#include "NodeUtils.h"

namespace MD
{
	namespace Node
	{
		void NodeExportBase::exportNodeTransitions(MR::NodeDef* nodeDef, ME::NodeExportXML* nodeExport)
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

		ME::NodeExportXML* NodeExportBase::exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName)
		{
			bool persistent = false;
			bool downstreamMultiplyConnected = false;

			if ((nodeDef->getNodeTypeID() == NODE_TYPE_NETWORK) || (nodeDef->getNodeTypeID() == NODE_TYPE_STATE_MACHINE) || NodeUtils::isNodeControlParameter(nodeDef))
				persistent = true;

			if (NodeUtils::isNodeControlParameter(nodeDef) || (nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_OUTPUT_REFERENCED))
				downstreamMultiplyConnected = true;

			ME::NodeExportXML* nodeExportXML = static_cast<ME::NodeExportXML*>(netDefExport->createNode(nodeDef->getNodeID(), nodeDef->getNodeTypeID(), nodeDef->getParentNodeID(), downstreamMultiplyConnected, nodeName.c_str(), persistent));
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			return nodeExportXML;
		}
	}
}