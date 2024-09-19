#pragma once
#include "assetProcessor/include/assetProcessor/BlendNodeBuilderUtils.h"
#include "extern.h"
#include "RCore.h"

namespace MorphemeExport
{
	namespace NodeExportHelpers
	{
		void writeSyncEventFlagsFromAttribData(ME::DataBlockExportXML* attribDataBlock, MR::AttribDataUInt* durationEventMatchingOpAttrib);
	}
}