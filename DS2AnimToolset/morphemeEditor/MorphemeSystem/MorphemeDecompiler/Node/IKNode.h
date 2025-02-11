#pragma once
#include "NodeExportBase.h"
#include "NMIK/NMPointIK.h"

namespace MD
{
	namespace Node
	{
		class NodeIKDecompilerBase : public NodeDecompilerBase
		{
		public:
			NodeIKDecompilerBase() {}
			~NodeIKDecompilerBase() {}

		protected:
			bool isAxisFlipped(NMP::Vector3 axisVector);
			float getAngleFromClampedCosine(float clampedCosine);
			uint32_t getUpAxisIndex(NMP::Vector3 upAxisVector);
			float getBias(NMRU::IKJointParams* jointParams, int numJointParams);
		};

		class NodeHeadLookDecompiler : public NodeIKDecompilerBase
		{
		public:
			NodeHeadLookDecompiler() {}
			~NodeHeadLookDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeHipsIKDecompiler : public NodeIKDecompilerBase
		{
		public:
			NodeHipsIKDecompiler() {}
			~NodeHipsIKDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeLockFootDecompiler : public NodeIKDecompilerBase
		{
		public:
			NodeLockFootDecompiler() {}
			~NodeLockFootDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeGunAimIKDecompiler : public NodeIKDecompilerBase
		{
		public:
			NodeGunAimIKDecompiler() {}
			~NodeGunAimIKDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeTwoBoneIKDecompiler : public NodeIKDecompilerBase
		{
		public:
			NodeTwoBoneIKDecompiler() {}
			~NodeTwoBoneIKDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeBasicUnevenTerrainDecompiler : public NodeIKDecompilerBase
		{
		public:
			NodeBasicUnevenTerrainDecompiler() {}
			~NodeBasicUnevenTerrainDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		protected:
			virtual void writeChainAttrib(ME::DataBlockExportXML* nodeDataBlock, uint32_t animSetIdx, MR::AttribDataBasicUnevenTerrainChain* chainAttrib);
		};

		class NodePredictiveUnevenTerrainDecompiler : public NodeBasicUnevenTerrainDecompiler
		{
		public:
			NodePredictiveUnevenTerrainDecompiler() {}
			~NodePredictiveUnevenTerrainDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			virtual void writeChainAttrib(ME::DataBlockExportXML* nodeDataBlock, uint32_t animSetIdx, MR::AttribDataPredictiveUnevenTerrainPredictionDef* chainAttrib);
		};
	}
}