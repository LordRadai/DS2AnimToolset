#pragma once
#include "NodeExportBase.h"

namespace MD
{
	namespace Node
	{
		class NodeExportCPFloat : public NodeExportBase
		{
		public:
			NodeExportCPFloat() {}
			~NodeExportCPFloat() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportCPVector3 : public NodeExportBase
		{
		public:
			NodeExportCPVector3() {}
			~NodeExportCPVector3() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportCPVector4 : public NodeExportBase
		{
		public:
			NodeExportCPVector4() {}
			~NodeExportCPVector4() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportCPBool : public NodeExportBase
		{
		public:
			NodeExportCPBool() {}
			~NodeExportCPBool() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportCPInt : public NodeExportBase
		{
		public:
			NodeExportCPInt() {}
			~NodeExportCPInt() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportCPUInt : public NodeExportBase
		{
		public:
			NodeExportCPUInt() {}
			~NodeExportCPUInt() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};
	}
}