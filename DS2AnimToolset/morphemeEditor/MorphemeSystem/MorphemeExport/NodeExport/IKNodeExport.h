#pragma once
#include "export/include/export/mcExportXML.h"
#include "morpheme/mrNodeDef.h"
#include "morpheme/mrNetworkDef.h"

namespace MorphemeExport
{
	namespace NodeExport
	{
		ME::NodeExportXML* exportHeadLookNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, MR::NodeDef* nodeDef);

	}
}