#include "NodeUtils.h"
#include "BlendNode.h"
#include "assetProcessor/include/assetProcessor/BlendNodeBuilderUtils.h"
#include "extern.h"
#include "RCore.h"
#include "MorphemeSystem/MorphemeUtils/MorphemeUtils.h"

namespace
{
	std::string getParentNodeName(std::string nodeName)
	{
		size_t pos = nodeName.rfind('|');

		if (pos != std::string::npos)
			return nodeName.substr(0, pos);

		return "";
	}

	std::string getCurrentNodeName(std::string nodeName)
	{
		size_t pos = nodeName.rfind('|');

		if (pos != std::string::npos)
			return nodeName.substr(pos + 1);

		return nodeName;
	}
}

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

			g_appLog->panicMessage("Input vector is not a valid axis vector (%.3f, %.3f, %.3f)\n", axis.x, axis.y, axis.z);
			return 0;
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

			if (nodeDef->getNodeTypeID() == NODE_TYPE_NETWORK)
				return "mainNetwork";

			std::string name = netDef->getNodeNameFromNodeID(nodeDef->getNodeID());

			std::string btName = "";

			if (isNodeBlendTree(netDef, nodeDef))
			{
				char tmp[256];
				sprintf_s(tmp, "BlendTree%d|", nodeDef->getNodeID());

				btName = tmp;
			}

			if (name == "")
			{
				if (nodeDef->getNodeTypeID() == NODE_TYPE_ANIM_EVENTS)
				{
					MR::AttribDataSourceAnim* sourceAnim = static_cast<MR::AttribDataSourceAnim*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM));

					assert(sourceAnim != nullptr);

					name = btName + RString::removeExtension(animLibrary->getAnimationSet(0)->getAnimationEntry(sourceAnim->m_animAssetID)->getAnimationFilename()) + std::to_string(nodeDef->getNodeID());
				}
				else if ((nodeDef->getNodeTypeID() == NODE_TYPE_TRANSIT) || (nodeDef->getNodeTypeID() == NODE_TYPE_TRANSIT_SYNC_EVENTS))
				{
					std::string srcName = "ActiveState1";

					if (nodeDef->getChildNodeID(0) != MR::INVALID_NODE_ID)
					{
						MR::NodeDef* srcNode = nodeDef->getChildNodeDef(0);

						if (isNodeBlendTree(netDef, srcNode))
							srcName = "BlendTree" + std::to_string(srcNode->getNodeID());
						else
							srcName = buildNodeName(netDef, srcNode, animLibrary);
					}

					std::string dstName = "ActiveState1";

					if (nodeDef->getChildNodeID(1) != MR::INVALID_NODE_ID)
					{
						MR::NodeDef* dstNode = nodeDef->getChildNodeDef(1);

						if (isNodeBlendTree(netDef, dstNode))
							dstName = "BlendTree" + std::to_string(dstNode->getNodeID());
						else
							dstName = buildNodeName(netDef, dstNode, animLibrary);
					}

					name = srcName + "_" + dstName;
				}
				else
				{
					const char* typeName = MorphemeUtils::getNodeTypeName(nodeDef->getNodeTypeID());

					char nodeName[256];
					sprintf_s(nodeName, "%s%s%d", btName.c_str(), typeName, nodeDef->getNodeID());

					name = nodeName;
				}
			}

			return name;
		}

		std::string buildNodeName(
			MR::NetworkDef* netDef,
			MR::NodeDef* nodeDef,
			ME::AnimationLibraryExport* animLibrary,
			std::map<MR::NodeID, std::string>& cachedNodeNames)
		{
			const MR::NodeID nodeID = nodeDef->getNodeID();

			// Fast path: cached
			auto it = cachedNodeNames.find(nodeID);
			if (it != cachedNodeNames.end() &&
				cachedNodeNames[nodeID] != "")
				return it->second;

			std::string name;

			// Control parameters have authoritative names
			if (isNodeControlParameter(nodeDef))
			{
				name = getCurrentNodeName(netDef->getNodeNameFromNodeID(nodeID));
				cachedNodeNames[nodeID] = name;
				return name;
			}

			// Network root
			if (nodeDef->getNodeTypeID() == NODE_TYPE_NETWORK)
			{
				name = "mainNetwork";
				cachedNodeNames[nodeID] = name;
				return name;
			}

			// Base name from network
			name = netDef->getNodeNameFromNodeID(nodeID);

			// Optional blend tree prefix
			std::string btPrefix;
			if (isNodeBlendTree(netDef, nodeDef))
			{
				char tmp[64];
				sprintf_s(tmp, "BlendTree%d|", nodeID);
				btPrefix = tmp;
			}

			// Generate fallback name if empty
			if (name.empty())
			{
				const MR::NodeType nodeType = nodeDef->getNodeTypeID();

				if (nodeType == NODE_TYPE_ANIM_EVENTS)
				{
					auto* sourceAnim =
						static_cast<MR::AttribDataSourceAnim*>(
							nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM));

					assert(sourceAnim != nullptr);

					const char* animFilename =
						animLibrary->getAnimationSet(0)
						->getAnimationEntry(sourceAnim->m_animAssetID)
						->getAnimationFilename();

					name = btPrefix
						+ RString::removeExtension(animFilename)
						+ std::to_string(nodeID);
				}
				else if (nodeType == NODE_TYPE_TRANSIT ||
					nodeType == NODE_TYPE_TRANSIT_SYNC_EVENTS)
				{
					auto resolveEndpointName = [&](uint32_t childIndex) -> std::string
						{
							if (nodeDef->getChildNodeID(childIndex) == MR::INVALID_NODE_ID)
								return "ActiveState1";

							MR::NodeDef* child = nodeDef->getChildNodeDef(childIndex);

							if (isNodeBlendTree(netDef, child))
								return "BlendTree" + std::to_string(child->getNodeID());

							return buildNodeName(netDef, child, animLibrary, cachedNodeNames);
						};

					std::string srcName = resolveEndpointName(0);
					std::string dstName = resolveEndpointName(1);

					name = srcName + "_" + dstName;
				}
				else
				{
					const char* typeName =
						MorphemeUtils::getNodeTypeName(nodeType);

					char nodeName[128];
					sprintf_s(nodeName, "%s%s%d",
						btPrefix.c_str(),
						typeName,
						nodeID);

					name = nodeName;
				}
			}

			// Store in cache before returning
			cachedNodeNames[nodeID] = name;
			return name;
		}


		std::string buildFullNodeName(MR::NetworkDef* netDef, MR::NodeDef* nodeDef, ME::AnimationLibraryExport* animLibrary, std::map<MR::NodeID, std::string>& cachedNodeNames)
		{
			std::string name = buildNodeName(netDef, nodeDef, animLibrary, cachedNodeNames);

			if (isNodeControlParameter(nodeDef))
				return "ControlParameters|" + name;

			MR::NodeDef* parentNodeContainer = getParentNodeContainer(netDef, nodeDef);

			while (parentNodeContainer != nullptr)
			{
				if (parentNodeContainer->getNodeTypeID() != NODE_TYPE_NETWORK)
				{
					std::string parentName = buildNodeName(netDef, parentNodeContainer, animLibrary, cachedNodeNames);

					if (isNodeBlendTree(netDef, parentNodeContainer))
					{
						char btName[256];
						sprintf_s(btName, "BlendTree%d", parentNodeContainer->getNodeID());

						parentName = btName;
					}

					name = parentName + "|" + name;
				}

				parentNodeContainer = getParentNodeContainer(netDef, parentNodeContainer);
			}

			return name;
		}

		MR::NodeDef* getParentNodeContainer(MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() == NODE_TYPE_NETWORK || isNodeControlParameter(nodeDef))
				return nullptr;

			const MR::NodeID parentNodeID = nodeDef->getParentNodeID();

			if (parentNodeID == MR::INVALID_NODE_ID)
				return nullptr;

			MR::NodeDef* nodeToCheck = netDef->getNodeDef(parentNodeID);

			while (nodeToCheck != nullptr)
			{
				if (isNodeContainer(netDef, nodeToCheck))
					return nodeToCheck;

				const MR::NodeID parentNodeID = nodeToCheck->getParentNodeID();

				if (parentNodeID == MR::INVALID_NODE_ID)
					break;

				nodeToCheck = netDef->getNodeDef(parentNodeID);
			}

			return nullptr;
		}

		bool isNodeBlendTree(MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			const MR::NodeID parentNodeID = nodeDef->getParentNodeID();

			if (parentNodeID == MR::INVALID_NODE_ID)
				return false;

			MR::NodeDef* parentNodeDef = netDef->getNodeDef(parentNodeID);
			
			if (parentNodeDef->getNodeTypeID() == NODE_TYPE_STATE_MACHINE &&
				nodeDef->getNodeTypeID() != NODE_TYPE_STATE_MACHINE)
				return true;

			return false;
		}

		bool isNodeContainer(MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (isNodeBlendTree(netDef, nodeDef))
				return true;

			return nodeDef->getNodeTypeID() == NODE_TYPE_NETWORK || nodeDef->getNodeTypeID() == NODE_TYPE_STATE_MACHINE;
		}

		void buildNodeNameMap(MR::NetworkDef* netDef, std::map<MR::NodeID, std::string>& cachedNodeNames)
		{
			for (size_t i = 0; i < netDef->getNumNodeDefs(); i++)
			{
				MR::NodeDef* nodeDef = netDef->getNodeDef(i);

				const std::string name = nodeDef->getName();

				if (name != "")
				{
					if (cachedNodeNames[nodeDef->getNodeID()] != getCurrentNodeName(name))
						cachedNodeNames[nodeDef->getNodeID()] = getCurrentNodeName(name);

					MR::NodeID parentNodeID = nodeDef->getParentNodeID();

					std::string currentName = name;
					while (parentNodeID != MR::INVALID_NODE_ID)
					{
						MR::NodeDef* parentNode = netDef->getNodeDef(parentNodeID);
						currentName = getParentNodeName(currentName);

						if (cachedNodeNames[parentNodeID] != getCurrentNodeName(currentName))
							cachedNodeNames[parentNodeID] = getCurrentNodeName(currentName);

						parentNodeID = parentNode->getParentNodeID();
					}
				}
				else
				{
					cachedNodeNames[nodeDef->getNodeID()] = name;
				}
			}
		}
	}
}