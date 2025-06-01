#pragma once
#include "NodeExportBase.h"

namespace MD
{
	namespace Node
	{
		class NodeCPFloatDecompiler : public NodeDecompilerBase
		{
		public:
			NodeCPFloatDecompiler() {}
			~NodeCPFloatDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeCPVector3Decompiler : public NodeDecompilerBase
		{
		public:
			NodeCPVector3Decompiler() {}
			~NodeCPVector3Decompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeCPVector4Decompiler : public NodeDecompilerBase
		{
		public:
			NodeCPVector4Decompiler() {}
			~NodeCPVector4Decompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeCPBoolDecompiler : public NodeDecompilerBase
		{
		public:
			NodeCPBoolDecompiler() {}
			~NodeCPBoolDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeCPIntDecompiler : public NodeDecompilerBase
		{
		public:
			NodeCPIntDecompiler() {}
			~NodeCPIntDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeCPUIntDecompiler : public NodeDecompilerBase
		{
		public:
			NodeCPUIntDecompiler() {}
			~NodeCPUIntDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};
	}
}