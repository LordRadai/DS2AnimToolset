#include "IKNodeExport.h"
#include "NodeExport.h"
#include "extern.h"
#include "RCore.h"
#include "morpheme/mrAttribData.h"
#include "NMIK/NMPointIK.h"

namespace MorphemeExport
{
	namespace NodeExport
	{
		ME::NodeExportXML* exportHeadLookNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_HEAD_LOOK)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_HEAD_LOOK, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeId(nodeDef->getChildNodeID(0), "InputNodeID");
			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(0)->m_sourceNodeID, nodeDef->getInputCPConnection(0)->m_sourcePinIndex, "Weight");
			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(1)->m_sourceNodeID, nodeDef->getInputCPConnection(1)->m_sourcePinIndex, "BlendWeight");

			MR::AttribDataHeadLookSetup* headLookSetup = static_cast<MR::AttribDataHeadLookSetup*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));
			
			nodeDataBlock->writeBool(headLookSetup->m_updateTargetByDeltas, "UpdateTargetByDeltas");
			nodeDataBlock->writeBool(headLookSetup->m_worldSpaceTarget, "WorldSpaceTarget");

			CHAR paramName[256];
			for (int animSetIndex = 0; animSetIndex < netDef->getNumAnimSets(); animSetIndex++)
			{
				MR::AttribDataHeadLookChain* headLookChain = static_cast<MR::AttribDataHeadLookChain*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET));

				if (animSetIndex == 0)
				{
					nodeDataBlock->writeBool(true, "ApplyJointLimits");
					nodeDataBlock->writeBool(headLookChain->m_ikParams->minimiseRotation, "MinimiseRotation");
					nodeDataBlock->writeBool(headLookChain->m_ikParams->keepUpright, "KeepUpright");

					nodeDataBlock->writeFloat(headLookChain->m_ikParams->worldUpAxis.x, "WorldUpAxisX");
					nodeDataBlock->writeFloat(headLookChain->m_ikParams->worldUpAxis.y, "WorldUpAxisY");
					nodeDataBlock->writeFloat(headLookChain->m_ikParams->worldUpAxis.z, "WorldUpAxisZ");
				}

				sprintf_s(paramName, "PointingVectorX_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(headLookChain->m_ikParams->pointingVector.x, paramName);
				sprintf_s(paramName, "PointingVectorY_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(headLookChain->m_ikParams->pointingVector.y, paramName);
				sprintf_s(paramName, "PointingVectorZ_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(headLookChain->m_ikParams->pointingVector.z, paramName);

				sprintf_s(paramName, "EndEffectorOffsetX_%d", animSetIndex + 1);
				nodeDataBlock->readFloat(headLookChain->m_endEffectorOffset.x, paramName);
				sprintf_s(paramName, "EndEffectorOffsetY_%d", animSetIndex + 1);
				nodeDataBlock->readFloat(headLookChain->m_endEffectorOffset.y, paramName);
				sprintf_s(paramName, "EndEffectorOffsetZ_%d", animSetIndex + 1);
				nodeDataBlock->readFloat(headLookChain->m_endEffectorOffset.z, paramName);

				sprintf_s(paramName, "Bias_%d", animSetIndex + 1);
				nodeDataBlock->writeFloat(headLookChain->m_ikParams->bias, paramName);

				sprintf_s(paramName, "EndJointIndex_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(headLookChain->m_endJointIndex, paramName);

				sprintf_s(paramName, "RootJointIndex_%d", animSetIndex + 1);
				nodeDataBlock->writeUInt(headLookChain->m_rootJointIndex, paramName);
			}

			return nodeExportXML;
		}
	}
}