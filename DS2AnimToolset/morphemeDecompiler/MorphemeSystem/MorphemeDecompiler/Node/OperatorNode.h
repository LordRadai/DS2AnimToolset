#pragma once
#include "NodeExportBase.h"

namespace MD
{
	namespace Node
	{
		class NodeOperatorFunctionDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorFunctionDecompiler() {}
			~NodeOperatorFunctionDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorArithmeticDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorArithmeticDecompiler() {}
			~NodeOperatorArithmeticDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorOneInputArithmeticDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorOneInputArithmeticDecompiler() {}
			~NodeOperatorOneInputArithmeticDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorFloatsToVector3Decompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorFloatsToVector3Decompiler() {}
			~NodeOperatorFloatsToVector3Decompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorFloatToIntDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorFloatToIntDecompiler() {}
			~NodeOperatorFloatToIntDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorIntToFloatDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorIntToFloatDecompiler() {}
			~NodeOperatorIntToFloatDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorNoiseGenDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorNoiseGenDecompiler() {}
			~NodeOperatorNoiseGenDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorSmoothFloatDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorSmoothFloatDecompiler() {}
			~NodeOperatorSmoothFloatDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			bool isScalar(MR::NodeDef* nodeDef);
		};

		class NodeOperatorRampFloatDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorRampFloatDecompiler() {}
			~NodeOperatorRampFloatDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorRandomFloatDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorRandomFloatDecompiler() {}
			~NodeOperatorRandomFloatDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorRateOfChangeDecompiler: public NodeDecompilerBase
		{
		public:
			NodeOperatorRateOfChangeDecompiler() {}
			~NodeOperatorRateOfChangeDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			bool isScalar(MR::NodeDef* nodeDef);
		};

		class NodeOperatorArithmeticVector3Decompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorArithmeticVector3Decompiler() {}
			~NodeOperatorArithmeticVector3Decompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorOneInputArithmeticVector3 : public NodeDecompilerBase
		{
		public:
			NodeOperatorOneInputArithmeticVector3() {}
			~NodeOperatorOneInputArithmeticVector3() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorVector3ToFloatsDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorVector3ToFloatsDecompiler() {}
			~NodeOperatorVector3ToFloatsDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorVector3DotDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorVector3DotDecompiler() {}
			~NodeOperatorVector3DotDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorVector3DistanceDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorVector3DistanceDecompiler() {}
			~NodeOperatorVector3DistanceDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorVector3CrossProductDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorVector3CrossProductDecompiler() {}
			~NodeOperatorVector3CrossProductDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorVector3AngleDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorVector3AngleDecompiler() {}
			~NodeOperatorVector3AngleDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorVector3NormaliseDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorVector3NormaliseDecompiler() {}
			~NodeOperatorVector3NormaliseDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeOperatorRayCastDecompiler : public NodeDecompilerBase
		{
		public:
			NodeOperatorRayCastDecompiler() {}
			~NodeOperatorRayCastDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};
	}
}