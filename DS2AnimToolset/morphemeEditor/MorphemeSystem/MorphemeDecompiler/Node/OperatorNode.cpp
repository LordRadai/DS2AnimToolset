#include "OperatorNode.h"
#include "Node.h"
#include "extern.h"
#include "RCore.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorSmoothFloat.h"

namespace MD
{
	namespace Node
	{
		bool isSmoothFloatScalar(MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_SMOOTH_FLOAT);
			
			MR::OutputCPTask outputCPTask = nodeDef->getOutputCPTask(0);

			if (outputCPTask == MR::nodeOperatorSmoothFloatCriticallyDampFloat)
				return false;
			else if (outputCPTask == MR::nodeOperatorSmoothFloatCriticallyDampVector)
				return false;
			else
				g_appLog->panicMessage("Invalid outputCPTaskFunction %s (nodeId=%d)\n", MR::Manager::getInstance().getOutputCPTaskName(outputCPTask), MR::Manager::getInstance().getOutputCPTaskName(outputCPTask), nodeDef->getNodeID());

			return false;
		}

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

		ME::NodeExportXML* exportOperatorSmoothFloatNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_SMOOTH_FLOAT);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			nodeDataBlock->writeNetworkNodeIdWithPinIndex(nodeDef->getInputCPConnection(0)->m_sourceNodeID, nodeDef->getInputCPConnection(0)->m_sourcePinIndex, "Input");

			MR::AttribDataSmoothFloatOperation* smoothFloatOperation = static_cast<MR::AttribDataSmoothFloatOperation*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeBool(isSmoothFloatScalar(nodeDef), "IsScalar");
			nodeDataBlock->writeBool(smoothFloatOperation->m_smoothVel, "SmoothVelocity");

			nodeDataBlock->writeFloat(smoothFloatOperation->m_floatRate, "SmoothTime");
			nodeDataBlock->writeFloat(smoothFloatOperation->m_initialValueX, "InitValue_X");
			nodeDataBlock->writeFloat(smoothFloatOperation->m_initialValueY, "InitValue_Y");
			nodeDataBlock->writeFloat(smoothFloatOperation->m_initialValueZ, "InitValue_Z");
			nodeDataBlock->writeBool(smoothFloatOperation->m_useInitValOnInit, "UseInitValueOnInit");

			return nodeExportXML;
		}
	}
}