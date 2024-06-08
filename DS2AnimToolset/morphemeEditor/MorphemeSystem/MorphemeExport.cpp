#include "MorphemeExport.h"

using namespace MR;

namespace MorphemeExport
{
	ME::TakeListXML* ExportAnimXML(MR::NodeDef* pNode, std::wstring animName)
	{
		if (pNode->getNodeID() != NODE_TYPE_ANIM_EVENTS)
			return nullptr;

		MR::AttribDataSourceAnim* sourceAnim = (MR::AttribDataSourceAnim*)pNode->getAttribData(ATTRIB_SEMANTIC_SOURCE_ANIM);
		MR::AttribDataSourceEventTrackSet* sourceEvents = (MR::AttribDataSourceEventTrackSet*)pNode->getAttribData(ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS);

		ME::ExportFactoryXML factory;

		std::wstring dstFilename = animName + L".xml";

		ME::TakeListXML* takeList = (ME::TakeListXML*)factory.createTakeList(animName.c_str(), dstFilename.c_str());
		ME::TakeExportXML* take = (ME::TakeExportXML*)takeList->createTake(L"untitled", sourceAnim->m_sourceAnimDuration, 30, false, sourceAnim->m_clipStartFraction, sourceAnim->m_clipEndFraction);
		
		
	}
}