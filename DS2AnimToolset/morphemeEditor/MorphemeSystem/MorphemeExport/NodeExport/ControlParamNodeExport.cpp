#include "ControlParamNodeExport.h"
#include "NodeExport.h"
#include "extern.h"
#include "RCore.h"

namespace MorphemeExport
{
	namespace NodeExport
	{
		ME::NodeExportXML* exportCPFloatNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_FLOAT)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_FLOAT, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataFloat* attribData = static_cast<MR::AttribDataFloat*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CP_FLOAT));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");

			nodeDataBlock->writeFloat(attribData->m_value, "DefaultValue_0");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportCPVector3Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_VECTOR3)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_VECTOR3, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataVector3* attribData = static_cast<MR::AttribDataVector3*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CP_VECTOR3));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");

			nodeDataBlock->writeFloat(attribData->m_value[0], "DefaultValue_0");
			nodeDataBlock->writeFloat(attribData->m_value[1], "DefaultValue_1");
			nodeDataBlock->writeFloat(attribData->m_value[2], "DefaultValue_2");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportCPVector4Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_VECTOR4)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_VECTOR4, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataVector4* attribData = static_cast<MR::AttribDataVector4*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CP_VECTOR4));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");

			nodeDataBlock->writeFloat(attribData->m_value[0], "DefaultValue_0");
			nodeDataBlock->writeFloat(attribData->m_value[1], "DefaultValue_1");
			nodeDataBlock->writeFloat(attribData->m_value[2], "DefaultValue_2");
			nodeDataBlock->writeFloat(attribData->m_value[3], "DefaultValue_3");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportCPBoolNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_BOOL)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_BOOL, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataBool* attribData = static_cast<MR::AttribDataBool*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CP_BOOL));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");
			nodeDataBlock->writeBool(attribData->m_value, "DefaultFlag");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportCPIntNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_INT)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_INT, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataInt* attribData = static_cast<MR::AttribDataInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CP_INT));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");
			nodeDataBlock->writeInt(attribData->m_value, "DefaultInt");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportCPUIntNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			if (nodeDef->getNodeTypeID() != NODE_TYPE_CP_UINT)
				g_appLog->panicMessage("Expecting node type %d (got %d)\n", NODE_TYPE_CP_UINT, nodeDef->getNodeTypeID());

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataUInt* attribData = static_cast<MR::AttribDataUInt*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_CP_UINT));

			if (attribData == nullptr)
				return nullptr;

			nodeDataBlock->writeUInt(0, "InputPathCount");
			nodeDataBlock->writeUInt(attribData->m_value, "DefaultInt");

			return nodeExportXML;
		}
	}
}