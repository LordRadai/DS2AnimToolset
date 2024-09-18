#pragma once
#include <string>
#include "../MRUtils/MRUtils.h"
#include "export/include/export/mcExportXML.h"
#include "export/include/export/mcExportMessagePresetsXml.h"
#include "morpheme/mrEventTrackDuration.h"
#include "morpheme/mrEventTrackDiscrete.h"
#include "morpheme/mrNodeDef.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
#include "export/include/export/mcExportControllerXml.h"
#include "MorphemeSystem/MorphemeCharacterDef/MorphemeCharacterDef.h"

namespace MorphemeExport
{
	ME::TakeListXML* exportAnimMarkup(MorphemeCharacterDef* character, int animId, std::wstring dstFileName);

	ME::CharacterControllerExportXML* exportCharacterController(MR::CharacterControllerDef* characterController, std::wstring dstFileName);
	ME::RigExportXML* exportRig(MR::NetworkDef* netDef, MR::AnimRigDef* rig, std::wstring dstFileName);
	ME::AnimationLibraryXML* exportAnimLibrary(MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* filenameLookup, MR::NetworkDef* netDef, std::vector<ME::RigExportXML*> rigExport, std::vector<ME::CharacterControllerExportXML*> controllerExport, std::wstring chrName, std::wstring dstFileName);
	ME::MessagePresetLibraryExportXML* exportMessagePresetLibrary(MR::NetworkDef* netDef, std::wstring chrName, std::wstring dstFileName);

	ME::NetworkDefExportXML* exportNetwork(MR::NetworkDef* netDef, ME::AnimationLibraryXML* animLibraryExport, ME::MessagePresetLibraryExportXML* messagePresetLibraryExport, std::wstring chrName, std::wstring dstFileName);

	namespace TransitExport
	{
		ME::ConditionSetExportXML* exportTransitConditionSet(ME::NodeExportXML* nodeExport, int targetNodeID, std::vector<unsigned int>& indices);
		ME::ConditionExportXML* exportTransitCondition(ME::NodeExportXML* nodeExport, MR::TransitConditionDef* transitCondDef);

		ME::ConditionSetExportXML* exportTransitCommonConditionSet(ME::NodeExportXML* nodeExport, int targetNodeID, std::vector<unsigned int>& indices);
		ME::ConditionExportXML* exportTransitCommonCondition(ME::NodeExportXML* nodeExport, MR::TransitConditionDef* transitCondDef);
	}

	namespace NodeExport
	{
		ME::NodeExportXML* exportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, int nodeId);
	}
}