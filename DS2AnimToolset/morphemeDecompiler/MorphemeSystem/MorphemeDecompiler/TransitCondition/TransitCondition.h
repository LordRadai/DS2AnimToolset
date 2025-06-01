#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamBoolSet.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamFloatGreater.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamFloatInRange.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamFloatLess.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamIntGreater.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamIntInRange.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamIntLess.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamUIntGreater.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamUIntInRange.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamUIntLess.h"
#include "morpheme/TransitConditions/mrTransitConditionCrossedCurveEventValueDecreasing.h"
#include "morpheme/TransitConditions/mrTransitConditionCrossedDurationFraction.h"
#include "morpheme/TransitConditions/mrTransitConditionDiscreteEventTriggered.h"
#include "morpheme/TransitConditions/mrTransitConditionFalse.h"
#include "morpheme/TransitConditions/mrTransitConditionInDurationEvent.h"
#include "morpheme/TransitConditions/mrTransitConditionInSyncEventRange.h"
#include "morpheme/TransitConditions/mrTransitConditionNodeActive.h"
#include "morpheme/TransitConditions/mrTransitConditionOnMessage.h"
#include "morpheme/TransitConditions/mrTransitConditionRayHit.h"

namespace MD
{
	namespace TransitCondition
	{
		class TransitConditionDecompilerBase
		{
		public:
			TransitConditionDecompilerBase() {}

			virtual void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport) = 0;
			virtual ~TransitConditionDecompilerBase() = default;
		};

		class TransitConditionCPBoolSetDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCPBoolSetDecompiler() {}
			~TransitConditionCPBoolSetDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCPBoolSet(static_cast<MR::TransitConditionDefControlParamBoolSet*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCPBoolSet(MR::TransitConditionDefControlParamBoolSet* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCPFloatInRangeDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCPFloatInRangeDecompiler() {}
			~TransitConditionCPFloatInRangeDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCPFloatInRange(static_cast<MR::TransitConditionDefControlParamFloatInRange*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCPFloatInRange(MR::TransitConditionDefControlParamFloatInRange* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCPIntInRangeDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCPIntInRangeDecompiler() {}
			~TransitConditionCPIntInRangeDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCPIntInRange(static_cast<MR::TransitConditionDefControlParamIntInRange*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCPIntInRange(MR::TransitConditionDefControlParamIntInRange* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCPUIntInRangeDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCPUIntInRangeDecompiler() {}
			~TransitConditionCPUIntInRangeDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCPUIntInRange(static_cast<MR::TransitConditionDefControlParamUIntInRange*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCPUIntInRange(MR::TransitConditionDefControlParamUIntInRange* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCPFloatLessDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCPFloatLessDecompiler() {}
			~TransitConditionCPFloatLessDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCPFloatLess(static_cast<MR::TransitConditionDefControlParamFloatLess*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCPFloatLess(MR::TransitConditionDefControlParamFloatLess* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCPFloatGreaterDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCPFloatGreaterDecompiler() {}
			~TransitConditionCPFloatGreaterDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCPFloatGreater(static_cast<MR::TransitConditionDefControlParamFloatGreater*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCPFloatGreater(MR::TransitConditionDefControlParamFloatGreater* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCPIntLessDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCPIntLessDecompiler() {}
			~TransitConditionCPIntLessDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCPIntLess(static_cast<MR::TransitConditionDefControlParamIntLess*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCPIntLess(MR::TransitConditionDefControlParamIntLess* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCPIntGreaterDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCPIntGreaterDecompiler() {}
			~TransitConditionCPIntGreaterDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCPIntGreater(static_cast<MR::TransitConditionDefControlParamIntGreater*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCPIntGreater(MR::TransitConditionDefControlParamIntGreater* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCPUIntLessDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCPUIntLessDecompiler() {}
			~TransitConditionCPUIntLessDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCPUIntLess(static_cast<MR::TransitConditionDefControlParamUIntLess*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCPUIntLess(MR::TransitConditionDefControlParamUIntLess* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCPUIntGreaterDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCPUIntGreaterDecompiler() {}
			~TransitConditionCPUIntGreaterDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCPUIntGreater(static_cast<MR::TransitConditionDefControlParamUIntGreater*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCPUIntGreater(MR::TransitConditionDefControlParamUIntGreater* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionInDurationEventDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionInDurationEventDecompiler() {}
			~TransitConditionInDurationEventDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionInDurationEvent(static_cast<MR::TransitConditionDefInDurationEvent*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionInDurationEvent(MR::TransitConditionDefInDurationEvent* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionInSyncEventSegmentDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionInSyncEventSegmentDecompiler() {}
			~TransitConditionInSyncEventSegmentDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionInSyncEventSegment(static_cast<MR::TransitConditionDefInSyncEventRange*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionInSyncEventSegment(MR::TransitConditionDefInSyncEventRange* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionInSyncEventRangeDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionInSyncEventRangeDecompiler() {}
			~TransitConditionInSyncEventRangeDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionInSyncEventRange(static_cast<MR::TransitConditionDefInSyncEventRange*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionInSyncEventRange(MR::TransitConditionDefInSyncEventRange* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCrossedSyncEventBoundaryDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCrossedSyncEventBoundaryDecompiler() {}
			~TransitConditionCrossedSyncEventBoundaryDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCrossedSyncEventBoundary(static_cast<MR::TransitConditionDefInSyncEventRange*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCrossedSyncEventBoundary(MR::TransitConditionDefInSyncEventRange* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionNodeActiveDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionNodeActiveDecompiler() {}
			~TransitConditionNodeActiveDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionNodeActive(static_cast<MR::TransitConditionDefNodeActive*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionNodeActive(MR::TransitConditionDefNodeActive* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionDiscreteEventTriggeredDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionDiscreteEventTriggeredDecompiler() {}
			~TransitConditionDiscreteEventTriggeredDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionDiscreteEventTriggered(static_cast<MR::TransitConditionDefDiscreteEventTriggered*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionDiscreteEventTriggered(MR::TransitConditionDefDiscreteEventTriggered* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCrossedDurationFractionDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCrossedDurationFractionDecompiler() {}
			~TransitConditionCrossedDurationFractionDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCrossedDurationFraction(static_cast<MR::TransitConditionDefCrossedDurationFraction*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCrossedDurationFraction(MR::TransitConditionDefCrossedDurationFraction* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionCrossedCurveEventValueDecreasingDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionCrossedCurveEventValueDecreasingDecompiler() {}
			~TransitConditionCrossedCurveEventValueDecreasingDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionCrossedCurveEventValueDecreasing(static_cast<MR::TransitConditionDefCrossedCurveEventValueDecreasing*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionCrossedCurveEventValueDecreasing(MR::TransitConditionDefCrossedCurveEventValueDecreasing* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionRayHitDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionRayHitDecompiler() {}
			~TransitConditionRayHitDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionRayHit(static_cast<MR::TransitConditionDefRayHit*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionRayHit(MR::TransitConditionDefRayHit* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionOnMessageDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionOnMessageDecompiler() {}
			~TransitConditionOnMessageDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
				this->exportTransitConditionOnMessage(static_cast<MR::TransitConditionDefOnMessage*>(transitConditionDef), conditionExport);
			}

		private:
			void exportTransitConditionOnMessage(MR::TransitConditionDefOnMessage* transitConditionDef, ME::ConditionExportXML* conditionExport);
		};

		class TransitConditionFalseDecompiler : public TransitConditionDecompilerBase
		{
		public:
			TransitConditionFalseDecompiler() {}
			~TransitConditionFalseDecompiler() {}

			void exportTransitCondition(MR::TransitConditionDef* transitConditionDef, ME::ConditionExportXML* conditionExport)
			{
			}
		};

		class TransitConditionDecompiler
		{
		public:
			TransitConditionDecompiler() {}
			~TransitConditionDecompiler() {}

			bool exportTransitCondition(MR::TransitConditionDef* transitCondDef, ME::ConditionExportXML* conditionExport, MR::TransitConditType type);
		};

		ME::ConditionSetExportXML* exportTransitConditionSet(ME::NodeExportXML* nodeExport, int targetNodeID, std::vector<unsigned int>& indices);
		ME::ConditionExportXML* exportTransitCondition(ME::NodeExportXML* nodeExport, MR::TransitConditionDef* transitCondDef);

		ME::ConditionSetExportXML* exportTransitCommonConditionSet(ME::NodeExportXML* nodeExport, int targetNodeID, std::vector<unsigned int>& indices);
		ME::ConditionExportXML* exportTransitCommonCondition(ME::NodeExportXML* nodeExport, MR::TransitConditionDef* transitCondDef);
	}
}