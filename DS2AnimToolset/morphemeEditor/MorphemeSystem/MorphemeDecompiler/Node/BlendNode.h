#pragma once
#include "NodeExportBase.h"

namespace MD
{
	namespace Node
	{
		class BlendNodeExportBase : public NodeExportBase
		{
		public:
			BlendNodeExportBase() {}
			~BlendNodeExportBase() {}

		protected:
			virtual void writeTimeStretchMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			virtual void writeBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock) {}
			virtual void writeEventBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			virtual void writeBlendFlags(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			virtual void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock) {}
			virtual void writePassThroughMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			virtual bool isBlendNodeWrapWeights(MR::NodeDef* nodeDef);
		};

		class NodeExportBlend2 : public BlendNodeExportBase
		{
		public:
			NodeExportBlend2() {}
			~NodeExportBlend2() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};

		class NodeExportBlend2x2 : public BlendNodeExportBase
		{
		public:
			NodeExportBlend2x2() {}
			~NodeExportBlend2x2() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};

		class NodeExportBlendN : public BlendNodeExportBase
		{
		public:
			NodeExportBlendN() {}
			~NodeExportBlendN() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};

		class NodeExportBlendNxM : public BlendNodeExportBase
		{
		public:
			NodeExportBlendNxM() {}
			~NodeExportBlendNxM() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};

		class NodeExportBlendAll : public BlendNodeExportBase
		{
		public:
			NodeExportBlendAll() {}
			~NodeExportBlendAll() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportFeatherBlend : public BlendNodeExportBase
		{
		public:
			NodeExportFeatherBlend() {}
			~NodeExportFeatherBlend() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeBlendMode(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};

		class NodeExportSubtractiveBlend : public BlendNodeExportBase
		{
		public:
			NodeExportSubtractiveBlend() {}
			~NodeExportSubtractiveBlend() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			void writeSlerpTrajPos(MR::NodeDef* nodeDef, ME::DataBlockExportXML* attribDataBlock);
		};
	}
}