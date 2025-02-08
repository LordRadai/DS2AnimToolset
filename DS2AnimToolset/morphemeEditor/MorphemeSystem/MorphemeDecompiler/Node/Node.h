#include "NodeExportBase.h"

namespace MD
{
	namespace Node
	{
		class NodeExportUnhandled : public NodeExportBase
		{
		public:
			NodeExportUnhandled() {}
			~NodeExportUnhandled() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportNetwork : public NodeExportBase
		{
		public:
			NodeExportNetwork() {}
			~NodeExportNetwork() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportAnimSyncEvents : public NodeExportBase
		{
		public:
			NodeExportAnimSyncEvents() {}
			~NodeExportAnimSyncEvents() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportMirrorTransforms : public NodeExportBase
		{
		public:
			NodeExportMirrorTransforms() {}
			~NodeExportMirrorTransforms() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportFilterTransforms : public NodeExportBase
		{
		public:
			NodeExportFilterTransforms() {}
			~NodeExportFilterTransforms() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportSingleFrame : public NodeExportBase
		{
		public:
			NodeExportSingleFrame() {}
			~NodeExportSingleFrame() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportFreeze : public NodeExportBase
		{
		public:
			NodeExportFreeze() {}
			~NodeExportFreeze() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);

		private:
			bool isPassThroughTransformsOnce(MR::NodeDef* nodeDef);
		};

		class NodeExportEmitRequestOnDiscreteEvent : public NodeExportBase
		{
		public:
			NodeExportEmitRequestOnDiscreteEvent() {}
			~NodeExportEmitRequestOnDiscreteEvent() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);

		private:
			MR::AttribDataEmittedMessagesMap::RequestType getRequestTypeID(MR::AttribDataEmittedMessagesMap* emittedMessageMap, uint32_t index);
		};

		class NodeExportPlaySpeedModifier : public NodeExportBase
		{
		public:
			NodeExportPlaySpeedModifier() {}
			~NodeExportPlaySpeedModifier() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportPassThrough : public NodeExportBase
		{
		public:
			NodeExportPassThrough() {}
			~NodeExportPassThrough() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportSmoothTransforms : public NodeExportBase
		{
		public:
			NodeExportSmoothTransforms() {}
			~NodeExportSmoothTransforms() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportSwitch : public NodeExportBase
		{
		public:
			NodeExportSwitch() {}
			~NodeExportSwitch() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportSequence : public NodeExportBase
		{
		public:
			NodeExportSequence() {}
			~NodeExportSequence() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportScaleToDuration : public NodeExportBase
		{
		public:
			NodeExportScaleToDuration() {}
			~NodeExportScaleToDuration() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportApplyGlobalTime : public NodeExportBase
		{
		public:
			NodeExportApplyGlobalTime() {}
			~NodeExportApplyGlobalTime() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportApplyBindPose : public NodeExportBase
		{
		public:
			NodeExportApplyBindPose() {}
			~NodeExportApplyBindPose() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportExtractJointInfo : public NodeExportBase
		{
		public:
			NodeExportExtractJointInfo() {}
			~NodeExportExtractJointInfo() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		
		private:
			bool getOutputSpace(MR::NodeDef* nodeDef);
		};

		class NodeExportClosestAnim : public NodeExportBase
		{
		public:
			NodeExportClosestAnim() {}
			~NodeExportClosestAnim() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportRetarget : public NodeExportBase
		{
		public:
			NodeExportRetarget() {}
			~NodeExportRetarget() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};

		class NodeExportScaleCharacter : public NodeExportBase
		{
		public:
			NodeExportScaleCharacter() {}
			~NodeExportScaleCharacter() {}

			ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef, std::string nodeName);
		};
	}
}