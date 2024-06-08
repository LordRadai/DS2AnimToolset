#pragma once
#include <string>
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrEventTrackDuration.h"
#include "morpheme/mrEventTrackDiscrete.h"
#include "morpheme/mrNodeDef.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/AnimSource/mrAnimSource.h"

namespace MorphemeExport
{
	ME::TakeListXML* ExportAnimXML(MR::NodeDef* pNode, std::wstring animName);
}