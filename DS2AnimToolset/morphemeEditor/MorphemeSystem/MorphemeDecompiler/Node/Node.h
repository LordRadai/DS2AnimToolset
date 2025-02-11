#include "NodeExportBase.h"

namespace MD
{
	namespace Node
	{
		class NodeUnhandledDecompiler : public NodeDecompilerBase
		{
		public:
			NodeUnhandledDecompiler() {}
			~NodeUnhandledDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeNetworkDecompiler : public NodeDecompilerBase
		{
		public:
			NodeNetworkDecompiler() {}
			~NodeNetworkDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeAnimSyncEventsDecompiler : public NodeDecompilerBase
		{
		public:
			NodeAnimSyncEventsDecompiler() {}
			~NodeAnimSyncEventsDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeMirrorTransformsDecompiler : public NodeDecompilerBase
		{
		public:
			NodeMirrorTransformsDecompiler() {}
			~NodeMirrorTransformsDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeFilterTransformsDecompiler : public NodeDecompilerBase
		{
		public:
			NodeFilterTransformsDecompiler() {}
			~NodeFilterTransformsDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeSingleFrameDecompiler : public NodeDecompilerBase
		{
		public:
			NodeSingleFrameDecompiler() {}
			~NodeSingleFrameDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeFreezeDecompiler : public NodeDecompilerBase
		{
		public:
			NodeFreezeDecompiler() {}
			~NodeFreezeDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);

		private:
			bool isPassThroughTransformsOnce(MR::NodeDef* nodeDef);
		};

		class NodeEmitRequestOnDiscreteEventDecompiler : public NodeDecompilerBase
		{
		public:
			NodeEmitRequestOnDiscreteEventDecompiler() {}
			~NodeEmitRequestOnDiscreteEventDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);

		private:
			MR::AttribDataEmittedMessagesMap::RequestType getRequestTypeID(MR::AttribDataEmittedMessagesMap* emittedMessageMap, uint32_t index);
		};

		class NodePlaySpeedModifierDecompiler : public NodeDecompilerBase
		{
		public:
			NodePlaySpeedModifierDecompiler() {}
			~NodePlaySpeedModifierDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodePassThroughDecompiler : public NodeDecompilerBase
		{
		public:
			NodePassThroughDecompiler() {}
			~NodePassThroughDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeSmoothTransformsDecompiler : public NodeDecompilerBase
		{
		public:
			NodeSmoothTransformsDecompiler() {}
			~NodeSmoothTransformsDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeSwitchDecompiler : public NodeDecompilerBase
		{
		public:
			NodeSwitchDecompiler() {}
			~NodeSwitchDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeSequenceDecompiler : public NodeDecompilerBase
		{
		public:
			NodeSequenceDecompiler() {}
			~NodeSequenceDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeScaleToDurationDecompiler : public NodeDecompilerBase
		{
		public:
			NodeScaleToDurationDecompiler() {}
			~NodeScaleToDurationDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeApplyGlobalTimeDecompiler : public NodeDecompilerBase
		{
		public:
			NodeApplyGlobalTimeDecompiler() {}
			~NodeApplyGlobalTimeDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeApplyBindPoseDecompiler : public NodeDecompilerBase
		{
		public:
			NodeApplyBindPoseDecompiler() {}
			~NodeApplyBindPoseDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExtractJointInfoDecompiler : public NodeDecompilerBase
		{
		public:
			NodeExtractJointInfoDecompiler() {}
			~NodeExtractJointInfoDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		
		private:
			bool getOutputSpace(MR::NodeDef* nodeDef);
		};

		class NodeClosestAnimDecompiler : public NodeDecompilerBase
		{
		public:
			NodeClosestAnimDecompiler() {}
			~NodeClosestAnimDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeRetargetDecompiler : public NodeDecompilerBase
		{
		public:
			NodeRetargetDecompiler() {}
			~NodeRetargetDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeScaleCharacterDecompiler : public NodeDecompilerBase
		{
		public:
			NodeScaleCharacterDecompiler() {}
			~NodeScaleCharacterDecompiler() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};
	}
}