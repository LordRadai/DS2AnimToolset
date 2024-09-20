#include "BlendNodeExport.h"
#include "assetProcessor/include/assetProcessor/BlendNodeBuilderUtils.h"
#include "extern.h"
#include "RCore.h"

namespace MorphemeExport
{
	namespace NodeExportHelpers
	{
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
					g_appLog->panicMessage("Invalid blend matching operation %d\n", durationEventMatchingOpAttrib->m_value);
					break;
				}
			}

			attribDataBlock->writeBool(durationEventBlendPassThrough, "DurationEventBlendPassThrough");
			attribDataBlock->writeBool(durationEventBlendInSequence, "DurationEventBlendInSequence");
			attribDataBlock->writeBool(durationEventBlendSameUserData, "DurationEventBlendSameUserData");
			attribDataBlock->writeBool(durationEventBlendOnOverlap, "DurationEventBlendOnOverlap");
			attribDataBlock->writeBool(durationEventBlendWithinRange, "DurationEventBlendWithinRange");
		}
	}
}