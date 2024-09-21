#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrAttribData.h"

namespace MD
{
	namespace NodeUtils
	{
		void writeSyncEventFlagsFromAttribData(ME::DataBlockExportXML* attribDataBlock, MR::AttribDataUInt* durationEventMatchingOpAttrib);
	}
}