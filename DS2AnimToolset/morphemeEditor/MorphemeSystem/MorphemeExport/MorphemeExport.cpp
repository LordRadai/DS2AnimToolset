#include "MorphemeExport.h"
#include "utils/RString/RString.h"

using namespace MR;

ME::TakeListXML* MorphemeExport::ExportAnimXML(CharacterDef* character, int animId, std::wstring dstFileName)
{
	MR::NodeDef* animNode = nullptr;

	MR::NetworkDef* netDef = character->getNetworkDef();
	int numNodes = netDef->getNumNodeDefs();

	for (int idx = 0; idx < numNodes; idx++)
	{
		MR::NodeDef* node = netDef->getNodeDef(idx);

		if (node->getNodeTypeID() == NODE_TYPE_ANIM_EVENTS)
		{
			MR::AttribDataSourceAnim* source_anim = (MR::AttribDataSourceAnim*)node->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM);

			if (source_anim && (source_anim->m_animAssetID == animId))
			{
				animNode = node;
				break;
			}
		}
	}

	if (animNode == nullptr)
		return nullptr;

	MR::AttribDataSourceAnim* sourceAnim = (MR::AttribDataSourceAnim*)animNode->getAttribData(ATTRIB_SEMANTIC_SOURCE_ANIM);
	MR::AttribDataSourceEventTrackSet* sourceEvents = (MR::AttribDataSourceEventTrackSet*)animNode->getAttribData(ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS);

	int sourceAnimId = sourceAnim->m_animAssetID;

	ME::ExportFactoryXML factory;

	std::wstring dest = dstFileName;

	ME::TakeListXML* takeList = (ME::TakeListXML*)factory.createTakeList(dest.c_str(), dest.c_str());
	ME::TakeExportXML* take = (ME::TakeExportXML*)takeList->createTake(RString::ToWide(character->getAnimFileLookUp()->getTakeName(sourceAnimId)).c_str(), sourceAnim->m_sourceAnimDuration, 30, false, sourceAnim->m_clipStartFraction, sourceAnim->m_clipEndFraction);

	for (size_t i = 0; i < sourceEvents->m_numDiscreteEventTracks; i++)
	{
		GUID gidReference;
		CoCreateGuid(&gidReference);

		MR::EventTrackDefDiscrete* track = sourceEvents->m_sourceDiscreteEventTracks[i];

		ME::DiscreteEventTrackExportXML* trackXML = (ME::DiscreteEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE, RString::GuidToString(gidReference).c_str(), RString::ToWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

		for (size_t j = 0; j < track->getNumEvents(); j++)
			trackXML->createEvent(j, track->getEvent(j)->getStartTime(), track->getEvent(j)->getUserData());
	}

	for (size_t i = 0; i < sourceEvents->m_numCurveEventTracks; i++)
	{
		GUID gidReference;
		CoCreateGuid(&gidReference);

		MR::EventTrackDefCurve* track = sourceEvents->m_sourceCurveEventTracks[i];

		ME::CurveEventTrackExportXML* trackXML = (ME::CurveEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_CURVE, RString::GuidToString(gidReference).c_str(), RString::ToWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

		for (size_t j = 0; j < track->getNumEvents(); j++)
			trackXML->createEvent(j, track->getEvent(j)->getTime(), track->getEvent(j)->getValue(), track->getEvent(j)->getUserData());
	}

	for (size_t i = 0; i < sourceEvents->m_numDurEventTracks; i++)
	{
		GUID gidReference;
		CoCreateGuid(&gidReference);

		MR::EventTrackDefDuration* track = sourceEvents->m_sourceDurEventTracks[i];

		ME::DurationEventTrackExportXML* trackXML = (ME::DurationEventTrackExportXML*)take->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION, RString::GuidToString(gidReference).c_str(), RString::ToWide(track->getTrackName()).c_str(), track->getTrackID(), track->getUserData());

		for (size_t j = 0; j < track->getNumEvents(); j++)
			trackXML->createEvent(j, track->getEvent(j)->getStartTime(), track->getEvent(j)->getDuration(), track->getEvent(j)->getUserData());
	}

	return takeList;
}