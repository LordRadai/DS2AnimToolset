#include "TransitConditionExport.h"
#include "extern.h"
#include "RCore.h"

namespace MorphemeExport
{
	namespace TransitConditionExport
	{
		void exportTransitConditionOnCPBoolSet(MR::TransitConditionDefControlParamBoolSet* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_CONTROL_PARAM_BOOL_SET_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_CONTROL_PARAM_BOOL_SET_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getTestValue(), "TestValue");
		}

		void exportTransitConditionCPFloatInRange(MR::TransitConditionDefControlParamFloatInRange* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getInvertFlag(), "NotInRange");

			dataBlock->writeString("float", "DataType");
			dataBlock->writeFloat(transitCondDef->getLowerTestValue(), "LowerTestValue");
			dataBlock->writeFloat(transitCondDef->getUpperTestValue(), "UpperTestValue");
		}

		void exportTransitConditionCPIntInRange(MR::TransitConditionDefControlParamIntInRange* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getInvertFlag(), "NotInRange");

			dataBlock->writeString("int", "DataType");
			dataBlock->writeInt(transitCondDef->getLowerTestValue(), "LowerTestValue");
			dataBlock->writeInt(transitCondDef->getUpperTestValue(), "UpperTestValue");
		}

		void exportTransitConditionCPUIntInRange(MR::TransitConditionDefControlParamUIntInRange* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getInvertFlag(), "NotInRange");

			dataBlock->writeString("uint", "DataType");
			dataBlock->writeUInt(transitCondDef->getLowerTestValue(), "LowerTestValue");
			dataBlock->writeUInt(transitCondDef->getUpperTestValue(), "UpperTestValue");
		}

		void exportTransitConditionCPFloatLess(MR::TransitConditionDefControlParamFloatLess* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(true, "LessThanOperation");

			dataBlock->writeString("float", "DataType");
			dataBlock->writeUInt(transitCondDef->getTestValue(), "TestValue");
		}

		void exportTransitConditionCPFloatGreater(MR::TransitConditionDefControlParamFloatGreater* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(false, "LessThanOperation");

			dataBlock->writeString("float", "DataType");
			dataBlock->writeUInt(transitCondDef->getTestValue(), "TestValue");
		}

		void exportTransitConditionCPIntLess(MR::TransitConditionDefControlParamIntLess* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_CONTROL_PARAM_INT_LESS_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_CONTROL_PARAM_INT_LESS_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(true, "LessThanOperation");

			dataBlock->writeString("int", "DataType");
			dataBlock->writeUInt(transitCondDef->getTestValue(), "TestValue");
		}

		void exportTransitConditionCPIntGreater(MR::TransitConditionDefControlParamIntGreater* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_CONTROL_PARAM_INT_GREATER_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_CONTROL_PARAM_INT_GREATER_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(false, "LessThanOperation");

			dataBlock->writeString("int", "DataType");
			dataBlock->writeUInt(transitCondDef->getTestValue(), "TestValue");
		}

		void exportTransitConditionCPUIntLess(MR::TransitConditionDefControlParamUIntLess* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_CONTROL_PARAM_INT_LESS_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_CONTROL_PARAM_INT_LESS_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(true, "LessThanOperation");

			dataBlock->writeString("uint", "DataType");
			dataBlock->writeUInt(transitCondDef->getTestValue(), "TestValue");
		}

		void exportTransitConditionCPUIntGreater(MR::TransitConditionDefControlParamUIntGreater* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_CONTROL_PARAM_INT_GREATER_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_CONTROL_PARAM_INT_GREATER_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(false, "LessThanOperation");

			dataBlock->writeString("uint", "DataType");
			dataBlock->writeUInt(transitCondDef->getTestValue(), "TestValue");
		}

		void exportTransitConditionInDurationEvent(MR::TransitConditionDefInDurationEvent* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_IN_DURATION_EVENT_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_IN_DURATION_EVENT_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			int eventTrackUserData = transitCondDef->getEventTrackUserData();
			int eventUserData = transitCondDef->getEventUserData();

			bool useEventTrackUserData = true;
			if (eventTrackUserData == MR::USER_DATA_UNSPECIFIED)
				useEventTrackUserData = false;

			bool useEventUserData = true;
			if (eventTrackUserData == MR::USER_DATA_UNSPECIFIED)
				useEventUserData = false;

			dataBlock->writeBool(useEventTrackUserData, "UseEventTrackUserTypeID");
			dataBlock->writeInt(eventTrackUserData, "EventTrackUserTypeID");
			dataBlock->writeBool(useEventUserData, "UseEventUserTypeID");
			dataBlock->writeInt(eventUserData, "EventUserTypeID");
			dataBlock->writeBool(transitCondDef->getInvertFlag(), "OnNotSet");
		}

		void exportTransitConditionOnMessage(MR::TransitConditionDefOnMessage* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			if (transitCondDef->getType() != TRANSCOND_ON_MESSAGE_ID)
				g_appLog->panicMessage("Wrong transit condition type. Expecting %d got %d", TRANSCOND_ON_MESSAGE_ID, transitCondDef->getType());

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeUInt(transitCondDef->getMessageID(), "MessageID");
			dataBlock->writeBool(transitCondDef->getInvertFlag(), "OnNotSet");
		}

		void exportTransitConditionDataBlock(MR::TransitConditionDef* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			switch (transitCondDef->getType())
			{
			case TRANSCOND_ON_MESSAGE_ID:
				exportTransitConditionOnMessage(static_cast<MR::TransitConditionDefOnMessage*>(transitCondDef), conditionExport);
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
			case TRANSCOND_FALSE_ID:
				// Handle case for TRANSCOND_FALSE_ID
				break;
			case TRANSCOND_CROSSED_CURVE_EVENT_VALUE_DECREASING_ID:
				// Handle case for TRANSCOND_CROSSED_CURVE_EVENT_VALUE_DECREASING_ID
				break;
			case TRANSCOND_IN_SYNC_EVENT_RANGE_ID:
				exportTransitConditionInDurationEvent(static_cast<MR::TransitConditionDefInDurationEvent*>(transitCondDef), conditionExport);
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
			case TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID:
				exportTransitConditionCPFloatGreater(static_cast<MR::TransitConditionDefControlParamFloatGreater*>(transitCondDef), conditionExport);
				break;
			case TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID:
				exportTransitConditionCPFloatLess(static_cast<MR::TransitConditionDefControlParamFloatLess*>(transitCondDef), conditionExport);
				break;
			case TRANSCOND_CONTROL_PARAM_UINT_GREATER_ID:
				exportTransitConditionCPUIntGreater(static_cast<MR::TransitConditionDefControlParamUIntGreater*>(transitCondDef), conditionExport);
				break;
			case TRANSCOND_CONTROL_PARAM_UINT_LESS_ID:
				exportTransitConditionCPUIntLess(static_cast<MR::TransitConditionDefControlParamUIntLess*>(transitCondDef), conditionExport);
				break;
			case TRANSCOND_CONTROL_PARAM_INT_GREATER_ID:
				exportTransitConditionCPIntGreater(static_cast<MR::TransitConditionDefControlParamIntGreater*>(transitCondDef), conditionExport);
				break;
			case TRANSCOND_CONTROL_PARAM_INT_LESS_ID:
				exportTransitConditionCPIntLess(static_cast<MR::TransitConditionDefControlParamIntLess*>(transitCondDef), conditionExport);
				break;
			case TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID:
				exportTransitConditionCPIntInRange(static_cast<MR::TransitConditionDefControlParamIntInRange*>(transitCondDef), conditionExport);
				break;
			case TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID:
				exportTransitConditionCPUIntInRange(static_cast<MR::TransitConditionDefControlParamUIntInRange*>(transitCondDef), conditionExport);
				break;
			case TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID:
				exportTransitConditionCPFloatInRange(static_cast<MR::TransitConditionDefControlParamFloatInRange*>(transitCondDef), conditionExport);
				break;
			case TRANSCOND_CONTROL_PARAM_BOOL_SET_ID:
				exportTransitConditionOnCPBoolSet(static_cast<MR::TransitConditionDefControlParamBoolSet*>(transitCondDef), conditionExport);
				break;
			default:
				g_appLog->panicMessage("Unknown TransitConditType %d", transitCondDef->getType());
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

			exportTransitConditionDataBlock(transitCondDef, conditionExport);

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

			exportTransitConditionDataBlock(transitCondDef, conditionExport);

			return conditionExport;
		}
	}
}