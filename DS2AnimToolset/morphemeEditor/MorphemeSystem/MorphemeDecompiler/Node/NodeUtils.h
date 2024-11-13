#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrAttribData.h"

namespace MD
{
	namespace NodeUtils
	{
		void writeSyncEventFlagsFromAttribData(ME::DataBlockExportXML* attribDataBlock, MR::AttribDataUInt* durationEventMatchingOpAttrib);
		void writeInputCPConnection(ME::DataBlockExportXML* attribDataBlock, const char* name, const MR::CPConnection* cpConnection, bool optional);
		bool isNodeControlParameter(MR::NodeDef* nodeDef);
	}
}