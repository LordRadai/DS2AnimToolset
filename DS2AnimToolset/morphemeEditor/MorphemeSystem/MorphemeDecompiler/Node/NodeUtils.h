#pragma once
#include <map>
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrNodeDef.h"

namespace MD
{
	namespace NodeUtils
	{
		int getAxisIndex(NMP::Vector3 axis);
		void writeSyncEventFlagsFromAttribData(ME::DataBlockExportXML* attribDataBlock, MR::AttribDataUInt* durationEventMatchingOpAttrib);
		void writeInputCPConnection(ME::DataBlockExportXML* attribDataBlock, const char* name, const MR::CPConnection* cpConnection, bool optional);
	}
}