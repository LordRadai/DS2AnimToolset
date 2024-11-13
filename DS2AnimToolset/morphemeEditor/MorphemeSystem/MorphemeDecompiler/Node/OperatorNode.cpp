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

			NodeUtils::writeInputCPConnection(nodeDataBlock, "Input", nodeDef->getInputCPConnection(0));

			MR::AttribDataFunctionOperation* functionOpCode = static_cast<MR::AttribDataFunctionOperation*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));
		
			nodeDataBlock->writeInt(functionOpCode->m_operation, "OperationCode");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorArithmeticNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_ARITHMETIC);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			NodeUtils::writeInputCPConnection(nodeDataBlock, "Input0", nodeDef->getInputCPConnection(0));
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Input1", nodeDef->getInputCPConnection(1));

			MR::AttribDataArithmeticOperation* functionOpCode = static_cast<MR::AttribDataArithmeticOperation*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeInt(functionOpCode->m_operation, "OperationCode");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorArithmeticVector3Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_ARITHMETIC_VECTOR3);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			NodeUtils::writeInputCPConnection(nodeDataBlock, "Input0", nodeDef->getInputCPConnection(0));
			NodeUtils::writeInputCPConnection(nodeDataBlock, "Input1", nodeDef->getInputCPConnection(1));

			MR::AttribDataArithmeticOperation* functionOpCode = static_cast<MR::AttribDataArithmeticOperation*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeInt(functionOpCode->m_operation, "OperationCode");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorFloatsToVector3Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_FLOATS_TO_VECTOR3);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			NodeUtils::writeInputCPConnection(nodeDataBlock, "FloatX", nodeDef->getInputCPConnection(0));
			NodeUtils::writeInputCPConnection(nodeDataBlock, "FloatY", nodeDef->getInputCPConnection(1));
			NodeUtils::writeInputCPConnection(nodeDataBlock, "FloatZ", nodeDef->getInputCPConnection(2));

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorFloatToIntNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_FLOAT_TO_INT);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			NodeUtils::writeInputCPConnection(nodeDataBlock, "Float", nodeDef->getInputCPConnection(0));

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorIntToFloatNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_INT_TO_FLOAT);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			NodeUtils::writeInputCPConnection(nodeDataBlock, "Int", nodeDef->getInputCPConnection(0));

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorNoiseGenNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_NOISE_GEN);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			NodeUtils::writeInputCPConnection(nodeDataBlock, "Input", nodeDef->getInputCPConnection(0));

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

			NodeUtils::writeInputCPConnection(nodeDataBlock, "Input", nodeDef->getInputCPConnection(0));

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

		ME::NodeExportXML* exportOperatorRandomFloatNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_RANDOM_FLOAT);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			MR::AttribDataRandomFloatDef* randomFloatDef = static_cast<MR::AttribDataRandomFloatDef*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeFloat(randomFloatDef->m_min, "Min");
			nodeDataBlock->writeFloat(randomFloatDef->m_max, "Max");
			nodeDataBlock->writeFloat(randomFloatDef->m_duration, "Interval");
			nodeDataBlock->writeUInt(randomFloatDef->m_seed, "Seed");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorOneInputArithmeticNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			NodeUtils::writeInputCPConnection(nodeDataBlock, "Input", nodeDef->getInputCPConnection(0));

			MR::AttribDataArithmeticOperation* functionOpCode = static_cast<MR::AttribDataArithmeticOperation*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeInt(functionOpCode->m_operation, "OperationCode");
			nodeDataBlock->writeFloat(functionOpCode->m_constValue, "ConstantValueX");

			return nodeExportXML;
		}

		ME::NodeExportXML* exportOperatorOneInputArithmeticVector3Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef)
		{
			THROW_NODE_TYPE_MISMATCH(nodeDef, NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3);

			ME::NodeExportXML* nodeExportXML = exportNodeCore(netDefExport, netDef, nodeDef);
			ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

			NodeUtils::writeInputCPConnection(nodeDataBlock, "Input", nodeDef->getInputCPConnection(0));

			MR::AttribDataArithmeticOperation* functionOpCode = static_cast<MR::AttribDataArithmeticOperation*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

			nodeDataBlock->writeInt(functionOpCode->m_operation, "OperationCode");
			nodeDataBlock->writeFloat(functionOpCode->m_constVector.x, "ConstantValueX");
			nodeDataBlock->writeFloat(functionOpCode->m_constVector.y, "ConstantValueY");
			nodeDataBlock->writeFloat(functionOpCode->m_constVector.z, "ConstantValueZ");

			return nodeExportXML;
		}
	}
}