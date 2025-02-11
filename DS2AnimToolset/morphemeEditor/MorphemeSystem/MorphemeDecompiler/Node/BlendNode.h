#pragma once
#include "NodeExportBase.h"

namespace MD
{
	namespace Node
	{
		class NodeBlendDecompilerBase : public NodeDecompilerBase
		{
		public:
			NodeBlendDecompilerBase() {}
			~NodeBlendDecompilerBase() {}

		protected:
			virtual void writeTimeStretchMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			virtual void writeBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock) {}
			virtual void writeEventBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			virtual void writeBlendFlags(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			virtual void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock) {}
			virtual void writePassThroughMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			virtual bool isBlendNodeWrapWeights(MR::NodeDef* nodeDef);
		};

		class NodeBlend2Decompiler : public NodeBlendDecompilerBase
		{
		public:
			NodeBlend2Decompiler() {}
			~NodeBlend2Decompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};

		class NodeBlend2x2Decompiler : public NodeBlendDecompilerBase
		{
		public:
			NodeBlend2x2Decompiler() {}
			~NodeBlend2x2Decompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};

		class NodeBlendNDecompiler : public NodeBlendDecompilerBase
		{
		public:
			NodeBlendNDecompiler() {}
			~NodeBlendNDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};

		class NodeBlendNxMDecompiler : public NodeBlendDecompilerBase
		{
		public:
			NodeBlendNxMDecompiler() {}
			~NodeBlendNxMDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};

		class NodeBlendAllDecompiler : public NodeBlendDecompilerBase
		{
		public:
			NodeBlendAllDecompiler() {}
			~NodeBlendAllDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeFeatherBlendDecompiler : public NodeBlendDecompilerBase
		{
		public:
			NodeFeatherBlendDecompiler() {}
			~NodeFeatherBlendDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};

		class NodeSubtractiveBlendDecompiler : public NodeBlendDecompilerBase
		{
		public:
			NodeSubtractiveBlendDecompiler() {}
			~NodeSubtractiveBlendDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};
	}
}