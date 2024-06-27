#pragma once
#include <string>
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrEventTrackDuration.h"
#include "morpheme/mrEventTrackDiscrete.h"
#include "morpheme/mrNodeDef.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "export/include/export/mcExportControllerXml.h"
#include "../CharacterDef/CharacterDef.h"

namespace MorphemeExport
{
	ME::TakeListXML* ExportAnimMarkup(CharacterDef* character, int animId, std::wstring dstFileName);

	ME::CharacterControllerExportXML* ExportCharacterController(MR::CharacterControllerDef* characterController, std::wstring dstFileName);
	ME::RigExportXML* ExportRig(MR::NetworkDef* netDef, MR::AnimRigDef* rig, std::wstring dstFileName);
	ME::NetworkDefExportXML* ExportNetwork(MR::NetworkDef* netDef, int chrId, std::wstring dstFileName);

	ME::NodeExportXML* ExportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, int nodeId);
	ME::NodeExportXML* ExportNodeCore(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	ME::NodeExportXML* ExportNodeUnhandled(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	ME::NodeExportXML* ExportNetworkNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	ME::NodeExportXML* ExportStateMachineNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	ME::NodeExportXML* ExportCPFloatNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	ME::NodeExportXML* ExportCPVector3Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	ME::NodeExportXML* ExportCPVector4Node(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	ME::NodeExportXML* ExportCPBoolNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	ME::NodeExportXML* ExportCPIntNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
	ME::NodeExportXML* ExportCPUIntNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);
}