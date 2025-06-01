#include "TransitCondition.h"
#include "extern.h"
#include "RCore.h"
#define THROW_NODE_TRANSIT_TYPE_MISMATCH(transitDef, type) if (transitDef->getType() != type) { g_appLog->panicMessage("Expecting node type %d (got %d)\n", type, transitDef->getType()); }

namespace MD
{
	namespace TransitCondition
	{
		void TransitConditionCPBoolSetDecompiler::exportTransitConditionCPBoolSet(MR::TransitConditionDefControlParamBoolSet* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CONTROL_PARAM_BOOL_SET_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getTestValue(), "TestValue");
		}

		void TransitConditionCPFloatInRangeDecompiler::exportTransitConditionCPFloatInRange(MR::TransitConditionDefControlParamFloatInRange* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getInvertFlag(), "NotInRange");

			dataBlock->writeString("float", "DataType");
			dataBlock->writeFloat(transitCondDef->getLowerTestValue(), "LowerTestValue");
			dataBlock->writeFloat(transitCondDef->getUpperTestValue(), "UpperTestValue");
		}

		void TransitConditionCPIntInRangeDecompiler::exportTransitConditionCPIntInRange(MR::TransitConditionDefControlParamIntInRange* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getInvertFlag(), "NotInRange");

			dataBlock->writeString("int", "DataType");
			dataBlock->writeInt(transitCondDef->getLowerTestValue(), "LowerTestValue");
			dataBlock->writeInt(transitCondDef->getUpperTestValue(), "UpperTestValue");
		}

		void TransitConditionCPUIntInRangeDecompiler::exportTransitConditionCPUIntInRange(MR::TransitConditionDefControlParamUIntInRange* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getInvertFlag(), "NotInRange");

			dataBlock->writeString("uint", "DataType");
			dataBlock->writeUInt(transitCondDef->getLowerTestValue(), "LowerTestValue");
			dataBlock->writeUInt(transitCondDef->getUpperTestValue(), "UpperTestValue");
		}

		void TransitConditionCPFloatLessDecompiler::exportTransitConditionCPFloatLess(MR::TransitConditionDefControlParamFloatLess* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(true, "LessThanOperation");

			dataBlock->writeString("float", "DataType");
			dataBlock->writeFloat(transitCondDef->getTestValue(), "TestValue");
		}

		void TransitConditionCPFloatGreaterDecompiler::exportTransitConditionCPFloatGreater(MR::TransitConditionDefControlParamFloatGreater* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(false, "LessThanOperation");

			dataBlock->writeString("float", "DataType");
			dataBlock->writeFloat(transitCondDef->getTestValue(), "TestValue");
		}

		void TransitConditionCPIntLessDecompiler::exportTransitConditionCPIntLess(MR::TransitConditionDefControlParamIntLess* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CONTROL_PARAM_INT_LESS_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(true, "LessThanOperation");

			dataBlock->writeString("int", "DataType");
			dataBlock->writeInt(transitCondDef->getTestValue(), "TestValue");
		}

		void TransitConditionCPIntGreaterDecompiler::exportTransitConditionCPIntGreater(MR::TransitConditionDefControlParamIntGreater* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CONTROL_PARAM_INT_GREATER_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(false, "LessThanOperation");

			dataBlock->writeString("int", "DataType");
			dataBlock->writeInt(transitCondDef->getTestValue(), "TestValue");
		}

		void TransitConditionCPUIntLessDecompiler::exportTransitConditionCPUIntLess(MR::TransitConditionDefControlParamUIntLess* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CONTROL_PARAM_UINT_LESS_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(true, "LessThanOperation");

			dataBlock->writeString("uint", "DataType");
			dataBlock->writeUInt(transitCondDef->getTestValue(), "TestValue");
		}

		void TransitConditionCPUIntGreaterDecompiler::exportTransitConditionCPUIntGreater(MR::TransitConditionDefControlParamUIntGreater* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CONTROL_PARAM_UINT_GREATER_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeNetworkNodeId(transitCondDef->getCPConnection()->m_sourceNodeID, "RuntimeNodeID");
			dataBlock->writeBool(transitCondDef->getOrEqual(), "OrEqual");
			dataBlock->writeBool(false, "LessThanOperation");

			dataBlock->writeString("uint", "DataType");
			dataBlock->writeUInt(transitCondDef->getTestValue(), "TestValue");
		}

		void TransitConditionInDurationEventDecompiler::exportTransitConditionInDurationEvent(MR::TransitConditionDefInDurationEvent* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_IN_DURATION_EVENT_ID);

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

		void TransitConditionInSyncEventSegmentDecompiler::exportTransitConditionInSyncEventSegment(MR::TransitConditionDefInSyncEventRange* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_IN_SYNC_EVENT_SEGMENT_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeFloat(transitCondDef->getEventRangeStart(), "EventIndex");
		}

		void TransitConditionInSyncEventRangeDecompiler::exportTransitConditionInSyncEventRange(MR::TransitConditionDefInSyncEventRange* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_IN_SYNC_EVENT_RANGE_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeFloat(transitCondDef->getEventRangeStart(), "EventRangeStart");
			dataBlock->writeFloat(transitCondDef->getEventRangeEnd(), "EventRangeEnd");
		}

		void TransitConditionCrossedSyncEventBoundaryDecompiler::exportTransitConditionCrossedSyncEventBoundary(MR::TransitConditionDefInSyncEventRange* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CROSSED_SYNC_EVENT_BOUNDARY_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeFloat(transitCondDef->getEventRangeStart(), "EventPosition");
		}

		void TransitConditionNodeActiveDecompiler::exportTransitConditionNodeActive(MR::TransitConditionDefNodeActive* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_NODE_ACTIVE_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeUInt(transitCondDef->getNodeID(), "NodeID");
		}

		void TransitConditionDiscreteEventTriggeredDecompiler::exportTransitConditionDiscreteEventTriggered(MR::TransitConditionDefDiscreteEventTriggered* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_DISCRETE_EVENT_TRIGGERED_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeUInt(transitCondDef->getEventUserDataTrigger(), "EventUserTypeID");
		}

		void TransitConditionCrossedDurationFractionDecompiler::exportTransitConditionCrossedDurationFraction(MR::TransitConditionDefCrossedDurationFraction* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CROSSED_DURATION_FRACTION_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeFloat(transitCondDef->getTriggerFraction(), "TestFraction");
		}

		void TransitConditionCrossedCurveEventValueDecreasingDecompiler::exportTransitConditionCrossedCurveEventValueDecreasing(MR::TransitConditionDefCrossedCurveEventValueDecreasing* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_CROSSED_CURVE_EVENT_VALUE_DECREASING_ID);

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
			dataBlock->writeFloat(transitCondDef->getMonitoringValue(), "TestValue");
		}

		void TransitConditionRayHitDecompiler::exportTransitConditionRayHit(MR::TransitConditionDefRayHit* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_RAY_HIT_ID);

			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeInt(transitCondDef->m_hitMode, "HitMode");
			dataBlock->writeBool(transitCondDef->m_useLocalOrientation, "UseLocalOrientation");
			dataBlock->writeFloat(transitCondDef->m_rayStartX, "RayStartX");
			dataBlock->writeFloat(transitCondDef->m_rayStartY, "RayStartY");
			dataBlock->writeFloat(transitCondDef->m_rayStartZ, "RayStartZ");
			dataBlock->writeFloat(transitCondDef->m_rayDeltaX, "RayDeltaX");
			dataBlock->writeFloat(transitCondDef->m_rayDeltaY, "RayDeltaY");
			dataBlock->writeFloat(transitCondDef->m_rayDeltaZ, "RayDeltaZ");
		}

		void TransitConditionOnMessageDecompiler::exportTransitConditionOnMessage(MR::TransitConditionDefOnMessage* transitCondDef, ME::ConditionExportXML* conditionExport)
		{
			THROW_NODE_TRANSIT_TYPE_MISMATCH(transitCondDef, TRANSCOND_ON_MESSAGE_ID);
			
			ME::DataBlockExportXML* dataBlock = static_cast<ME::DataBlockExportXML*>(conditionExport->getDataBlock());

			dataBlock->writeUInt(transitCondDef->getMessageID(), "MessageID");
			dataBlock->writeBool(transitCondDef->getInvertFlag(), "OnNotSet");
		}

		bool TransitConditionDecompiler::exportTransitCondition(MR::TransitConditionDef* transitCondDef, ME::ConditionExportXML* conditionExport, MR::TransitConditType type)
		{
			TransitConditionDecompilerBase* transitCondDecompiler = nullptr;

			switch (type)
			{
			case TRANSCOND_ON_MESSAGE_ID:
				transitCondDecompiler = new TransitConditionOnMessageDecompiler;
				break;
			case TRANSCOND_DISCRETE_EVENT_TRIGGERED_ID:
				transitCondDecompiler = new TransitConditionDiscreteEventTriggeredDecompiler;
				break;
			case TRANSCOND_CROSSED_DURATION_FRACTION_ID:
				transitCondDecompiler = new TransitConditionCrossedDurationFractionDecompiler;
				break;
			case TRANSCOND_CROSSED_SYNC_EVENT_BOUNDARY_ID:
				transitCondDecompiler = new TransitConditionCrossedSyncEventBoundaryDecompiler;
				break;
			case TRANSCOND_IN_SYNC_EVENT_SEGMENT_ID:
				transitCondDecompiler = new TransitConditionInSyncEventSegmentDecompiler;
				break;
			case TRANSCOND_FALSE_ID:
				transitCondDecompiler = new TransitConditionFalseDecompiler;
				break;
			case TRANSCOND_CROSSED_CURVE_EVENT_VALUE_DECREASING_ID:
				transitCondDecompiler = new TransitConditionCrossedCurveEventValueDecreasingDecompiler;
				break;
			case TRANSCOND_IN_SYNC_EVENT_RANGE_ID:
				transitCondDecompiler = new TransitConditionInSyncEventRangeDecompiler;
				break;
			case TRANSCOND_RAY_HIT_ID:
				transitCondDecompiler = new TransitConditionRayHitDecompiler;
				break;
			case TRANSCOND_NODE_ACTIVE_ID:
				transitCondDecompiler = new TransitConditionNodeActiveDecompiler;
				break;
			case TRANSCOND_IN_DURATION_EVENT_ID:
				transitCondDecompiler = new TransitConditionInDurationEventDecompiler;
				break;
			case TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID:
				transitCondDecompiler = new TransitConditionCPFloatGreaterDecompiler;
				break;
			case TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID:
				transitCondDecompiler = new TransitConditionCPFloatLessDecompiler;
				break;
			case TRANSCOND_CONTROL_PARAM_UINT_GREATER_ID:
				transitCondDecompiler = new TransitConditionCPUIntGreaterDecompiler;
				break;
			case TRANSCOND_CONTROL_PARAM_UINT_LESS_ID:
				transitCondDecompiler = new TransitConditionCPUIntLessDecompiler;
				break;
			case TRANSCOND_CONTROL_PARAM_INT_GREATER_ID:
				transitCondDecompiler = new TransitConditionCPIntGreaterDecompiler;
				break;
			case TRANSCOND_CONTROL_PARAM_INT_LESS_ID:
				transitCondDecompiler = new TransitConditionCPIntLessDecompiler;
				break;
			case TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID:
				transitCondDecompiler = new TransitConditionCPIntInRangeDecompiler;
				break;
			case TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID:
				transitCondDecompiler = new TransitConditionCPUIntInRangeDecompiler;
				break;
			case TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID:
				transitCondDecompiler = new TransitConditionCPFloatInRangeDecompiler;
				break;
			case TRANSCOND_CONTROL_PARAM_BOOL_SET_ID:
				transitCondDecompiler = new TransitConditionCPBoolSetDecompiler;
				break;
			default:
				g_appLog->panicMessage("Unknown TransitConditType %d", transitCondDef->getType());
				return false;
			}

			if (transitCondDecompiler != nullptr)
			{
				transitCondDecompiler->exportTransitCondition(transitCondDef, conditionExport);

				delete transitCondDecompiler;
			}

			return true;
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

			if ((type == TRANSCOND_CONTROL_PARAM_UINT_GREATER_ID) || (type == TRANSCOND_CONTROL_PARAM_UINT_LESS_ID) || (type == TRANSCOND_CONTROL_PARAM_INT_GREATER_ID) || (type == TRANSCOND_CONTROL_PARAM_INT_LESS_ID) || (type == TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID) || (type == TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID))
				type = TRANSCOND_CONTROL_PARAM_TEST_ID;
			else if ((type == TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID) || (type == TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID) || (type == TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID))
				type = TRANSCOND_CONTROL_PARAM_IN_RANGE_ID;

			ME::ConditionExportXML* conditionExport = static_cast<ME::ConditionExportXML*>(nodeExport->createCondition(idx, type));

			TransitConditionDecompiler decompiler;
			decompiler.exportTransitCondition(transitCondDef, conditionExport, transitCondDef->getType());

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

			if ((type == TRANSCOND_CONTROL_PARAM_UINT_GREATER_ID) || (type == TRANSCOND_CONTROL_PARAM_UINT_LESS_ID) || (type == TRANSCOND_CONTROL_PARAM_INT_GREATER_ID) || (type == TRANSCOND_CONTROL_PARAM_INT_LESS_ID) || (type == TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID) || (type == TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID))
				type = TRANSCOND_CONTROL_PARAM_TEST_ID;
			else if ((type == TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID) || (type == TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID) || (type == TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID))
				type = TRANSCOND_CONTROL_PARAM_IN_RANGE_ID;

			ME::ConditionExportXML* conditionExport = static_cast<ME::ConditionExportXML*>(nodeExport->createCommonCondition(idx, type));

			TransitConditionDecompiler decompiler;
			decompiler.exportTransitCondition(transitCondDef, conditionExport, transitCondDef->getType());

			return conditionExport;
		}
	}
}