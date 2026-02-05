#include "NodeUtils.h"
#include "BlendNode.h"
#include "assetProcessor/include/assetProcessor/BlendNodeBuilderUtils.h"
#include "extern.h"
#include "RCore.h"
#include "MorphemeSystem/MorphemeUtils/MorphemeUtils.h"

namespace MD
{
	namespace NodeUtils
	{
		int getAxisIndex(NMP::Vector3 axis)
		{
			if (axis.x)
				return 1;
			else if (axis.y)
				return 2;
			else if (axis.z)
				return 3;

			INVOKE_PANIC("Input vector is not a valid axis vector (%.3f, %.3f, %.3f)\n", axis.x, axis.y, axis.z);
		}

		void writeSyncEventFlagsFromAttribData(ME::DataBlockExportXML* attribDataBlock, MR::AttribDataUInt* durationEventMatchingOpAttrib)
		{
			bool durationEventBlendPassThrough = false;
			bool durationEventBlendInSequence = false;
			bool durationEventBlendSameUserData = false;
			bool durationEventBlendOnOverlap = false;
			bool durationEventBlendWithinRange = false;

			if (durationEventMatchingOpAttrib)
			{
				switch (durationEventMatchingOpAttrib->m_value)
				{
				case DURATION_EVENT_MATCH_PASS_THROUGH:
					durationEventBlendPassThrough = true;
					break;
				case DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS_ON_OVERLAP:
					durationEventBlendInSequence = true;
					durationEventBlendSameUserData = true;
					durationEventBlendOnOverlap = true;
					break;
				case DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS_WITHIN_RANGE:
					durationEventBlendInSequence = true;
					durationEventBlendSameUserData = true;
					durationEventBlendWithinRange = true;
					break;
				case DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS:
					durationEventBlendInSequence = true;
					durationEventBlendSameUserData = true;
					break;
				case DURATION_EVENT_MATCH_IN_SEQUENCE_ON_OVERLAP:
					durationEventBlendInSequence = true;
					durationEventBlendOnOverlap = true;
					break;
				case DURATION_EVENT_MATCH_IN_SEQUENCE_WITHIN_RANGE:
					durationEventBlendInSequence = true;
					durationEventBlendWithinRange = true;
					break;
				case DURATION_EVENT_MATCH_IN_SEQUENCE:
					durationEventBlendInSequence = true;
					break;
				case DURATION_EVENT_MATCH_SAME_IDS_ON_OVERLAP:
					durationEventBlendSameUserData = true;
					durationEventBlendOnOverlap = true;
					break;
				case DURATION_EVENT_MATCH_SAME_IDS_WITHIN_RANGE:
					durationEventBlendSameUserData = true;
					durationEventBlendWithinRange = true;
					break;
				case DURATION_EVENT_MATCH_SAME_IDS:
					durationEventBlendSameUserData = true;
					break;
				case DURATION_EVENT_MATCH_ON_OVERLAP:
					durationEventBlendOnOverlap = true;
					break;
				case DURATION_EVENT_MATCH_WITHIN_RANGE:
					durationEventBlendWithinRange = true;
					break;
				default:
					INVOKE_PANIC("Invalid blend matching operation %d\n", durationEventMatchingOpAttrib->m_value);
					break;
				}
			}

			attribDataBlock->writeBool(durationEventBlendPassThrough, "DurationEventBlendPassThrough");
			attribDataBlock->writeBool(durationEventBlendInSequence, "DurationEventBlendInSequence");
			attribDataBlock->writeBool(durationEventBlendSameUserData, "DurationEventBlendSameUserData");
			attribDataBlock->writeBool(durationEventBlendOnOverlap, "DurationEventBlendOnOverlap");
			attribDataBlock->writeBool(durationEventBlendWithinRange, "DurationEventBlendWithinRange");
		}

		void writeInputCPConnection(ME::DataBlockExportXML* nodeDataBlock, const char* name, const MR::CPConnection* cpConnection, bool optional)
		{
			if (!optional)
				nodeDataBlock->writeNetworkNodeIdWithPinIndex(cpConnection->m_sourceNodeID, cpConnection->m_sourcePinIndex, name);
			else if ((cpConnection->m_sourceNodeID != MR::INVALID_NODE_ID) && (cpConnection->m_sourcePinIndex != MR::INVALID_PIN_INDEX))
				nodeDataBlock->writeNetworkNodeIdWithPinIndex(cpConnection->m_sourceNodeID, cpConnection->m_sourcePinIndex, name);
		}
	}
}