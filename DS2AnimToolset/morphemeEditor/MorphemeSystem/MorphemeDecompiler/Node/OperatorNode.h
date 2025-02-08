#pragma once
#include "NodeExportBase.h"

namespace MD
{
	namespace Node
	{
		class NodeExportOperatorFunction : public NodeExportBase
		{
		public:
			NodeExportOperatorFunction() {}
			~NodeExportOperatorFunction() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorArithmetic : public NodeExportBase
		{
		public:
			NodeExportOperatorArithmetic() {}
			~NodeExportOperatorArithmetic() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorOneInputArithmetic : public NodeExportBase
		{
		public:
			NodeExportOperatorOneInputArithmetic() {}
			~NodeExportOperatorOneInputArithmetic() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorFloatsToVector3 : public NodeExportBase
		{
		public:
			NodeExportOperatorFloatsToVector3() {}
			~NodeExportOperatorFloatsToVector3() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorFloatToInt : public NodeExportBase
		{
		public:
			NodeExportOperatorFloatToInt() {}
			~NodeExportOperatorFloatToInt() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorIntToFloat : public NodeExportBase
		{
		public:
			NodeExportOperatorIntToFloat() {}
			~NodeExportOperatorIntToFloat() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorNoiseGen : public NodeExportBase
		{
		public:
			NodeExportOperatorNoiseGen() {}
			~NodeExportOperatorNoiseGen() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorSmoothFloat : public NodeExportBase
		{
		public:
			NodeExportOperatorSmoothFloat() {}
			~NodeExportOperatorSmoothFloat() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			bool isScalar(MR::NodeDef* nodeDef);
		};

		class NodeExportOperatorRampFloat : public NodeExportBase
		{
		public:
			NodeExportOperatorRampFloat() {}
			~NodeExportOperatorRampFloat() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorRandomFloat : public NodeExportBase
		{
		public:
			NodeExportOperatorRandomFloat() {}
			~NodeExportOperatorRandomFloat() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorRateOfChange: public NodeExportBase
		{
		public:
			NodeExportOperatorRateOfChange() {}
			~NodeExportOperatorRateOfChange() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		private:
			bool isScalar(MR::NodeDef* nodeDef);
		};

		class NodeExportOperatorArithmeticVector3 : public NodeExportBase
		{
		public:
			NodeExportOperatorArithmeticVector3() {}
			~NodeExportOperatorArithmeticVector3() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorOneInputArithmeticVector3 : public NodeExportBase
		{
		public:
			NodeExportOperatorOneInputArithmeticVector3() {}
			~NodeExportOperatorOneInputArithmeticVector3() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorVector3ToFloats : public NodeExportBase
		{
		public:
			NodeExportOperatorVector3ToFloats() {}
			~NodeExportOperatorVector3ToFloats() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorVector3Dot : public NodeExportBase
		{
		public:
			NodeExportOperatorVector3Dot() {}
			~NodeExportOperatorVector3Dot() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorVector3Distance : public NodeExportBase
		{
		public:
			NodeExportOperatorVector3Distance() {}
			~NodeExportOperatorVector3Distance() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorVector3CrossProduct : public NodeExportBase
		{
		public:
			NodeExportOperatorVector3CrossProduct() {}
			~NodeExportOperatorVector3CrossProduct() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorVector3Angle : public NodeExportBase
		{
		public:
			NodeExportOperatorVector3Angle() {}
			~NodeExportOperatorVector3Angle() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorVector3Normalise : public NodeExportBase
		{
		public:
			NodeExportOperatorVector3Normalise() {}
			~NodeExportOperatorVector3Normalise() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportOperatorRayCast : public NodeExportBase
		{
		public:
			NodeExportOperatorRayCast() {}
			~NodeExportOperatorRayCast() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};
	}
}