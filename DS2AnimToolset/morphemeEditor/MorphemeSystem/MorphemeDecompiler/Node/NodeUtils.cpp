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
	
		void writeInputCPConnection(ME::DataBlockExportXML* nodeDataBlock, const char* name, const MR::CPConnection* cpConnection, bool optional)
		{
			if (!optional)
				nodeDataBlock->writeNetworkNodeIdWithPinIndex(cpConnection->m_sourceNodeID, cpConnection->m_sourcePinIndex, name);
			else if ((cpConnection->m_sourceNodeID != MR::INVALID_NODE_ID) && (cpConnection->m_sourcePinIndex != MR::INVALID_PIN_INDEX))
				nodeDataBlock->writeNetworkNodeIdWithPinIndex(cpConnection->m_sourceNodeID, cpConnection->m_sourcePinIndex, name);
		}

		bool isNodeControlParameter(MR::NodeDef* nodeDef)
		{
			if ((nodeDef->getNodeTypeID() >= 20) && (nodeDef->getNodeTypeID() <= 26))
				return true;

			return false;
		}

		std::string buildNodeName(MR::NetworkDef* netDef, MR::NodeDef* nodeDef, ME::AnimationLibraryExport* animLibrary)
		{
			if (isNodeControlParameter(nodeDef))
				return netDef->getNodeNameFromNodeID(nodeDef->getNodeID());

			std::string name;

			if ((nodeDef->getNodeTypeID() != NODE_TYPE_TRANSIT) && (nodeDef->getNodeTypeID() != NODE_TYPE_TRANSIT_SYNC_EVENTS))
			{
				for (size_t i = 0; i < nodeDef->getNumChildNodes(); i++)
				{
					MR::NodeDef* childNode = nodeDef->getChildNodeDef(i);

					if (childNode->getNodeTypeID() == NODE_TYPE_STATE_MACHINE)
						name = netDef->getNodeNameFromNodeID(childNode->getNodeID());

					if (name != "")
						break;
				}
			}

			if (name == "")
			{
				if (nodeDef->getNodeTypeID() == NODE_TYPE_ANIM_EVENTS)
				{
					MR::AttribDataSourceAnim* sourceAnim = static_cast<MR::AttribDataSourceAnim*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM));

					assert(sourceAnim != nullptr);

					return RString::removeExtension(animLibrary->getAnimationSet(0)->getAnimationEntry(sourceAnim->m_animAssetID)->getAnimationFilename()) + "_" + std::to_string(nodeDef->getNodeID());
				}
				else if ((nodeDef->getNodeTypeID() == NODE_TYPE_TRANSIT) || (nodeDef->getNodeTypeID() == NODE_TYPE_TRANSIT_SYNC_EVENTS))
				{
					std::string srcName = "ActiveState";

					if (nodeDef->getChildNodeID(0) != MR::INVALID_NODE_ID)
						srcName = buildNodeName(netDef, nodeDef->getChildNodeDef(0), animLibrary);

					std::string dstName = "ActiveState";

					if (nodeDef->getChildNodeID(1) != MR::INVALID_NODE_ID)
						dstName = buildNodeName(netDef, nodeDef->getChildNodeDef(1), animLibrary);
										
					return srcName + "_" + dstName;
				}

				const char* typeName = MorphemeUtils::getNodeTypeName(nodeDef->getNodeTypeID());

				char nodeName[256];
				sprintf_s(nodeName, "%s_%d", typeName, nodeDef->getNodeID());

				name = nodeName;
			}

			return name;
		}
	}
}