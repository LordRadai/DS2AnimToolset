#include "MorphemeExport.h"
#include "utils/RString/RString.h"

using namespace MR;

MR::NodeDef* getAnimNodeByAnimID(MR::NetworkDef* netDef, int assetId)
{
	int numNodes = netDef->getNumNodeDefs();

	for (int idx = 0; idx < numNodes; idx++)
	{
		MR::NodeDef* node = netDef->getNodeDef(idx);

		if (node->getNodeTypeID() == NODE_TYPE_ANIM_EVENTS)
		{
			MR::AttribDataSourceAnim* source_anim = (MR::AttribDataSourceAnim*)node->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM);

			if (source_anim && (source_anim->m_animAssetID == assetId))
				return node;
		}
	}

	return nullptr;
}

ME::TakeListXML* MorphemeExport::ExportAnimMarkup(CharacterDef* character, int animId, std::wstring dstFileName)
{
	MR::NetworkDef* netDef = character->getNetworkDef();

	MR::NodeDef* animNode = getAnimNodeByAnimID(netDef, animId);

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

ME::NetworkDefExportXML* MorphemeExport::ExportNetwork(MR::NetworkDef* netDef, int chrId, std::wstring dstFileName)
{
	ME::ExportFactoryXML factory;

	GUID gidReference;
	CoCreateGuid(&gidReference);

	wchar_t netName[260];
	swprintf_s(netName, L"%04d", chrId);

	ME::NetworkDefExportXML* netDefExport = static_cast<ME::NetworkDefExportXML*>(factory.createNetworkDef(RString::GuidToString(gidReference).c_str(), netName, dstFileName.c_str()));

	netDefExport->setNetworkWorldOrientation(NMP::Vector3XAxis(), NMP::Vector3ZAxis(), NMP::Vector3XAxis());

	for (size_t i = 0; i < netDef->getNumNodeDefs(); i++)
	{
		MorphemeExport::ExportNode(netDefExport, netDef, i);
	}

	const NMP::IDMappedStringTable* messageTable = netDef->getMessageIDNamesTable();

	for (size_t i = 0; i < netDef->getNumMessages(); i++)
	{
		int messageId = netDef->getMessageDistributor(i)->m_messageID;
		int messageType = netDef->getMessageDistributor(i)->m_messageType;

		netDefExport->createMessage(RString::ToWide(netDef->getMessageNameFromMessageID(messageId)).c_str(), messageType, messageId);
	}

	netDefExport->setRootNodeNetworkID(netDef->getRootNodeID());

	return netDefExport;
}

ME::NodeExportXML* MorphemeExport::ExportNode(ME::NetworkDefExportXML* netDefExport, MR::NetworkDef* netDef, int nodeId)
{
	MR::NodeDef* nodeDef = netDef->getNodeDef(nodeId);

	ME::NodeExportXML* nodeExportXML = static_cast<ME::NodeExportXML*>(netDefExport->createNode(nodeDef->getNodeID(), nodeDef->getNodeTypeID(), nodeDef->getParentNodeID(), false, netDef->getNodeNameFromNodeID(nodeDef->getNodeID())));
	ME::DataBlockExportXML* nodeDataBlock = static_cast<ME::DataBlockExportXML*>(nodeExportXML->getDataBlock());

	MR::NodeType nodeTypeID = nodeDef->getNodeTypeID();

	switch (nodeTypeID)
	{
	default:
		break;
	}
}