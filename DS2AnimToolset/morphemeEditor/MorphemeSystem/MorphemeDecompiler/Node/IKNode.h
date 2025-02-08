#pragma once
#include "NodeExportBase.h"
#include "NMIK/NMPointIK.h"

namespace MD
{
	namespace Node
	{
		class IKNodeExportBase : public NodeExportBase
		{
		public:
			IKNodeExportBase() {}
			~IKNodeExportBase() {}

		protected:
			bool isAxisFlipped(NMP::Vector3 axisVector);
			float getAngleFromClampedCosine(float clampedCosine);
			uint32_t getUpAxisIndex(NMP::Vector3 upAxisVector);
			float getBias(NMRU::IKJointParams* jointParams, int numJointParams);
		};

		class NodeExportHeadLook : public IKNodeExportBase
		{
		public:
			NodeExportHeadLook() {}
			~NodeExportHeadLook() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportHipsIK : public IKNodeExportBase
		{
		public:
			NodeExportHipsIK() {}
			~NodeExportHipsIK() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportLockFoot : public IKNodeExportBase
		{
		public:
			NodeExportLockFoot() {}
			~NodeExportLockFoot() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportGunAimIK : public IKNodeExportBase
		{
		public:
			NodeExportGunAimIK() {}
			~NodeExportGunAimIK() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportTwoBoneIK : public IKNodeExportBase
		{
		public:
			NodeExportTwoBoneIK() {}
			~NodeExportTwoBoneIK() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportBasicUnevenTerrain : public IKNodeExportBase
		{
		public:
			NodeExportBasicUnevenTerrain() {}
			~NodeExportBasicUnevenTerrain() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		protected:
			void writeChainAttrib(ME::DataBlockExportXML* nodeDataBlock, uint32_t animSetIdx, MR::AttribDataBasicUnevenTerrainChain* chainAttrib);
		};

		class NodeExportPredictiveUnevenTerrain : public NodeExportBasicUnevenTerrain
		{
		public:
			NodeExportPredictiveUnevenTerrain() {}
			~NodeExportPredictiveUnevenTerrain() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeChainAttrib(ME::DataBlockExportXML* nodeDataBlock, uint32_t animSetIdx, MR::AttribDataPredictiveUnevenTerrainPredictionDef* chainAttrib);
		};
	}
}