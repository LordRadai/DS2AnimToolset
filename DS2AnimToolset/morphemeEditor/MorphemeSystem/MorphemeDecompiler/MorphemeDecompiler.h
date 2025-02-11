#pragma once
#include <string>
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

namespace MD
{
	ME::TakeListXML* exportAnimMarkup(MorphemeCharacterDef* character, int animId, std::wstring dstFileName, int fps);

	ME::CharacterControllerExportXML* exportCharacterController(MR::CharacterControllerDef* characterController, std::wstring dstFileName);
	ME::RigExportXML* exportRig(MR::NetworkDef* netDef, MR::AnimRigDef* rig, int animSetIdx, std::wstring dstFileName);
	ME::AnimationLibraryXML* exportAnimLibrary(MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* filenameLookup, MR::NetworkDef* netDef, std::vector<ME::RigExportXML*> rigExport, std::vector<ME::CharacterControllerExportXML*> controllerExport, std::wstring chrName, std::wstring dstFileName, const char* animFormat = "nsa", bool resample = false, int sampleFreq = 30);
	ME::MessagePresetLibraryExportXML* exportMessagePresetLibrary(MR::NetworkDef* netDef, std::wstring chrName, std::wstring dstFileName);

	ME::NetworkDefExportXML* exportNetwork(MR::NetworkDef* netDef, ME::AnimationLibraryXML* animLibraryExport, ME::MessagePresetLibraryExportXML* messagePresetLibraryExport, std::wstring chrName, std::wstring dstFileName);
}