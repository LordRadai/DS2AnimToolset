#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrAttribData.h"

namespace MorphemeExport
{
	namespace NodeExportHelpers
	{
		void writeSyncEventFlagsFromAttribData(ME::DataBlockExportXML* attribDataBlock, MR::AttribDataUInt* durationEventMatchingOpAttrib);
	}
}