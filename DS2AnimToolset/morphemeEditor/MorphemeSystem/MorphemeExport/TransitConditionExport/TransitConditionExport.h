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

namespace MorphemeExport
{
	namespace TransitConditionExport
	{
		ME::ConditionSetExportXML* exportTransitConditionSet(ME::NodeExportXML* nodeExport, int targetNodeID, std::vector<unsigned int>& indices);
		ME::ConditionExportXML* exportTransitCondition(ME::NodeExportXML* nodeExport, MR::TransitConditionDef* transitCondDef);

		ME::ConditionSetExportXML* exportTransitCommonConditionSet(ME::NodeExportXML* nodeExport, int targetNodeID, std::vector<unsigned int>& indices);
		ME::ConditionExportXML* exportTransitCommonCondition(ME::NodeExportXML* nodeExport, MR::TransitConditionDef* transitCondDef);
	}
}