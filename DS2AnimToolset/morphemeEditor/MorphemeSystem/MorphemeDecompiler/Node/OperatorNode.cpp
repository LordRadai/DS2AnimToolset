#include "OperatorNode.h"
#include "Node.h"
#include "extern.h"
#include "RCore.h"
#include "morpheme/mrAttribData.h"

namespace MD
{
	namespace Node
	{
		ME::NodeExportXML* exportOperatorFunctionNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_FUNCTION);
			
			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(0)->m_sourceNodeID, nodeDef->getInputCPConnection(0)->m_sourcePinIndex, "Input");

			MR::AttribDataFunctionOperation* functionOpCode = static_cast<MR::AttribDataFunctionOperation*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));
		
			nodeDataBlock->writeInt(functionOpCode->m_operation, "OperationCode");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorArithmeticNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_ARITHMETIC);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(0)->m_sourceNodeID, nodeDef->getInputCPConnection(0)->m_sourcePinIndex, "Input0");
			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(1)->m_sourceNodeID, nodeDef->getInputCPConnection(1)->m_sourcePinIndex, "Input1");

			MR::AttribDataArithmeticOperation* functionOpCode = static_cast<MR::AttribDataArithmeticOperation*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeInt(functionOpCode->m_operation, "OperationCode");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorArithmeticVector3Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_ARITHMETIC_VECTOR3);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(0)->m_sourceNodeID, nodeDef->getInputCPConnection(0)->m_sourcePinIndex, "Input0");
			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(1)->m_sourceNodeID, nodeDef->getInputCPConnection(1)->m_sourcePinIndex, "Input1");

			MR::AttribDataArithmeticOperation* functionOpCode = static_cast<MR::AttribDataArithmeticOperation*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeInt(functionOpCode->m_operation, "OperationCode");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorFloatsToVector3Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_FLOATS_TO_VECTOR3);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(0)->m_sourceNodeID, nodeDef->getInputCPConnection(0)->m_sourcePinIndex, "FloatX");
			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(1)->m_sourceNodeID, nodeDef->getInputCPConnection(1)->m_sourcePinIndex, "FloatY");
			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(2)->m_sourceNodeID, nodeDef->getInputCPConnection(2)->m_sourcePinIndex, "FloatZ");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorFloatToIntNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_FLOAT_TO_INT);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(0)->m_sourceNodeID, nodeDef->getInputCPConnection(0)->m_sourcePinIndex, "Float");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorIntToFloatNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_INT_TO_FLOAT);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(0)->m_sourceNodeID, nodeDef->getInputCPConnection(0)->m_sourcePinIndex, "Int");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorNoiseGenNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_NOISE_GEN);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(0)->m_sourceNodeID, nodeDef->getInputCPConnection(0)->m_sourcePinIndex, "Input");

			MR::AttribDataNoiseGenDef* noiseGenDef = static_cast<MR::AttribDataNoiseGenDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeFloat(noiseGenDef->m_noiseSawFrequency, "NoiseSawFrequency");
			nodeDataBlock->writeInt(noiseGenDef->m_flags, "NoiseFlags");

			return nodeExportXML;
		}
	}
}