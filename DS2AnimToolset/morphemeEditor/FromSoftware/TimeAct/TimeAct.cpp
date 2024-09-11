#include <filesystem>
#include <assert.h>
#include "TimeAct.h"
#include "framework.h"
#include "extern.h"
#include "RCore.h"

namespace TimeAct
{
	namespace
	{
		std::vector<TimeActEvent*> getEventList(TimeActData* taeData)
		{
			std::vector<TimeActEvent*> events;

			for (size_t i = 0; i < taeData->getNumEventGroups(); i++)
			{
				for (size_t j = 0; j < taeData->getEventGroup(i)->getNumEvents(); j++)
					events.push_back(taeData->getEventGroup(i)->getEvent(j));
			}

			return events;
		}

		ptrdiff_t findEventOffset(ptrdiff_t eventsOffset, std::vector<TimeActEvent*> events, TimeActEvent* event)
		{
			int eventIndex = -1;
			for (size_t i = 0; i < events.size(); i++)
			{
				if (events[i] == event)
					eventIndex = i;
			}

			if (eventIndex == -1)
				return NULL;

			return eventsOffset + eventIndex * sizeof(TAE3::TimeActEvent);
		}

		bool compareTracks(TimeActTrack* first, TimeActTrack* second)
		{
			return first->getId() < second->getId();
		}

		bool compareTaeGroup(TimeActTrackGroup* first, TimeActTrackGroup* second)
		{
			return first->getStartId() < second->getStartId();
		}
	}

	AnimData* AnimData::create(int lenght, int fps)
	{
		AnimData* animData = new AnimData;

		animData->m_fps = fps;
		animData->m_lenght = lenght;

		return animData;
	}

	AnimData* AnimData::createFromResource(TAE3::AnimData* resource)
	{
		AnimData* animData = new AnimData;

		animData->m_reference = resource->reference;
		animData->m_fps = resource->fps;
		animData->m_lenght = resource->lenght;

		return animData;
	}

	int AnimData::getMemoryRequirements()
	{
		return sizeof(TAE3::AnimData);
	}

	TAE3::AnimData AnimData::generateBinary(RFile* file)
	{
		int startPos = file->tell();

		TAE3::AnimData animData;

		animData.reference = 0;
		animData.sectionStart = (void*)(startPos + RMemory::getOffset(&animData.sectionEnd, &animData));
		animData.sectionEnd = (void*)((ptrdiff_t)animData.sectionStart + 0x20);
		animData.bVar18 = 0;
		animData.fps = this->m_fps;
		animData.bVar1A = 0;
		animData.bVar1B = 0;
		animData.lenght = this->m_lenght;
		animData.iVar20 = 0;
		animData.iVar28 = 0;

		file->write(animData);

		return animData;
	}

	TimeActData* TimeActData::create(int lenght, int fps)
	{
		TimeActData* taeData = new TimeActData;

		taeData->m_animData = AnimData::create(lenght, fps);

		return taeData;
	}

	TimeActData* TimeActData::createFromResource(TAE3::TimeActData* resource, TaeTemplate* taeTemplate)
	{
		TimeActData* taeData = new TimeActData;

		taeData->m_eventGroupCount = resource->eventGroupCount;

		taeData->m_eventGroups.reserve(resource->eventGroupCount);

		for (size_t i = 0; i < resource->eventGroupCount; i++)
			taeData->m_eventGroups.push_back(EventGroup::createFromResource(&resource->eventGroups[i], taeTemplate));

		taeData->m_animData = AnimData::createFromResource(resource->animData);

		std::sort(taeData->m_eventGroups.begin(), taeData->m_eventGroups.end(), EventGroup::lessThan);

		return taeData;
	}

	void TimeActData::destroy()
	{
		for (size_t i = 0; i < this->m_eventGroupCount; i++)
		{
			if (this->m_eventGroups[i])
			{
				this->m_eventGroups[i]->destroy();
				delete this->m_eventGroups[i];
			}
		}

		this->m_eventGroups.clear();

		if (this->m_animData)
			delete this->m_animData;
	}

	int TimeActData::getMemoryRequirements()
	{
		int size = sizeof(TAE3::TimeActData) + this->m_animData->getMemoryRequirements();

		for (size_t i = 0; i < this->m_eventGroupCount; i++)
		{
			int timesSize = 0;

			if (this->m_eventGroups[i]->m_numEvents)
				timesSize += RMemory::align(2 * sizeof(float) * this->m_eventGroups[i]->m_numEvents, 16);

			size += timesSize;

			size += this->m_eventGroups[i]->getMemoryRequirements();

			for (size_t j = 0; j < this->m_eventGroups[i]->m_numEvents; j++)
				size += this->m_eventGroups[i]->m_events[j]->getMemoryRequirements();
		}

		return size;
	}

	TAE3::TimeActData TimeActData::generateBinary(RFile* file, ptrdiff_t animDataOffset)
	{
		file->alignStream(16);
		int startPos = file->tell();

		TAE3::TimeActData taeData;

		int eventsSize = 0;

		taeData.eventGroupCount = this->m_eventGroupCount;
		taeData.eventCount = 0;
		taeData.timeCount = 0;
		for (size_t i = 0; i < this->m_eventGroupCount; i++)
		{
			taeData.eventCount += this->m_eventGroups[i]->getNumEvents();
			taeData.timeCount += 2 * this->m_eventGroups[i]->getNumEvents();

			for (size_t j = 0; j < this->m_eventGroups[i]->m_numEvents; j++)
				eventsSize += RMemory::align(this->m_eventGroups[i]->m_events[j]->getMemoryRequirements(), 16);
		}

		ptrdiff_t timesOffset = animDataOffset + sizeof(TAE3::AnimData);
		ptrdiff_t eventsOffset = RMemory::align(timesOffset + sizeof(float) * taeData.timeCount, 16);
		ptrdiff_t eventGroupsOffset = RMemory::align(eventsOffset + eventsSize, 16);

		if (taeData.eventCount)
			taeData.events = (TAE3::TimeActEvent*)(eventsOffset);
		else
			taeData.events = NULL;

		if (taeData.eventGroupCount)
			taeData.eventGroups = (TAE3::EventGroup*)(eventGroupsOffset);
		else
			taeData.eventGroups = NULL;

		if (taeData.timeCount)
			taeData.times = (float*)(timesOffset);
		else
			taeData.times = NULL;

		taeData.animData = (TAE3::AnimData*)(animDataOffset);

		file->write(taeData);

		return taeData;
	}

	TimeActTrack* TimeActTrack::create(int id, int lenght, int fps)
	{
		TimeActTrack* tae = new TimeActTrack;

		tae->m_id = id;
		tae->m_timeActData = TimeActData::create(lenght, fps);

		return tae;
	}

	TimeActTrack* TimeActTrack::createFromResource(TAE3::TimeActTrack* resource, TaeTemplate* taeTemplate)
	{
		TimeActTrack* tae = new TimeActTrack;

		tae->m_id = resource->id;
		tae->m_timeActData = TimeActData::createFromResource(resource->timeActData, taeTemplate);

		return tae;
	}

	void TimeActTrack::destroy()
	{
		if (this->m_timeActData)
		{
			this->m_timeActData->destroy();
			delete this->m_timeActData;
		}
	}

	EventGroup* TimeActTrack::addGroup(int groupTypeId)
	{
		EventGroup* group = EventGroup::create(groupTypeId);

		this->m_timeActData->m_eventGroupCount++;
		this->m_timeActData->m_eventGroups.push_back(group);

		std::sort(this->m_timeActData->m_eventGroups.begin(), this->m_timeActData->m_eventGroups.end(), EventGroup::lessThan);

		return group;
	}

	bool TimeActTrack::deleteGroup(int idx)
	{
		if (idx > this->m_timeActData->m_eventGroupCount)
			return false;

		this->m_timeActData->m_eventGroupCount--;

		EventGroup* group = this->m_timeActData->m_eventGroups[idx];
		delete group;

		this->m_timeActData->m_eventGroups.erase(this->m_timeActData->m_eventGroups.begin() + idx);

		std::sort(this->m_timeActData->m_eventGroups.begin(), this->m_timeActData->m_eventGroups.end(), EventGroup::lessThan);

		return true;
	}

	TimeActEvent* TimeActTrack::addEvent(int groupIdx, float startTime, float endTime, int value, TaeTemplate* taeTemplate)
	{
		if (groupIdx > this->m_timeActData->m_eventGroupCount)
			return nullptr;

		return this->m_timeActData->m_eventGroups[groupIdx]->addEvent(startTime, endTime, value, taeTemplate);
	}

	bool TimeActTrack::deleteEvent(int groupIdx, int eventIdx)
	{
		if (groupIdx > this->m_timeActData->m_eventGroupCount)
			return false;

		return this->m_timeActData->m_eventGroups[groupIdx]->deleteEvent(eventIdx);
	}

	int TimeActTrack::getMemoryRequirements()
	{
		return sizeof(TAE3::TimeActTrack);
	}

	TAE3::TimeActTrack TimeActTrack::generateBinary(RFile* file, ptrdiff_t taeDataOffset)
	{
		TAE3::TimeActTrack track;

		track.id = this->m_id;
		track.timeActData = (TAE3::TimeActData*)(taeDataOffset);

		file->write(track);

		return track;
	}

	TimeActTrackGroup* TimeActTrackGroup::create(int startTae, int endTae, TimeActTrack* source)
	{
		TimeActTrackGroup* group = new TimeActTrackGroup;

		group->m_taeStartId = startTae;
		group->m_taeEndId = endTae;
		group->m_tae = source;

		return group;
	}

	TimeActTrackGroup* TimeActTrackGroup::createFromResource(TAE3::TimeActGroup* resource)
	{
		TimeActTrackGroup* group = new TimeActTrackGroup;

		group->m_taeStartId = resource->taeStartId;
		group->m_taeEndId = resource->taeEndId;

		return group;
	}

	int TimeActTrackGroup::getMemoryRequirements()
	{
		return sizeof(TAE3::TimeActGroup);
	}

	TAE3::TimeActGroup TimeActTrackGroup::generateBinary(RFile* file, ptrdiff_t offset)
	{
		file->alignStream(16);
		int startPos = file->tell();

		TAE3::TimeActGroup taeGroup;

		taeGroup.taeStartId = this->m_taeStartId;
		taeGroup.taeEndId = this->m_taeEndId;
		taeGroup.timeActTracks = (TAE3::TimeActTrack*)offset;

		file->write(taeGroup);

		return taeGroup;
	}

	TimeActLookupTable* TimeActLookupTable::create()
	{
		TimeActLookupTable* lookupTable = new TimeActLookupTable;
		lookupTable->m_groupCount = 0;

		return lookupTable;
	}

	TimeActLookupTable* TimeActLookupTable::createFromResource(TAE3::TimeActLookupTable* resource)
	{
		TimeActLookupTable* lookupTable = new TimeActLookupTable;

		lookupTable->m_groupCount = resource->groupCount;

		lookupTable->m_groups.reserve(resource->groupCount);

		for (size_t i = 0; i < resource->groupCount; i++)
			lookupTable->m_groups.push_back(TimeActTrackGroup::createFromResource(&resource->groups[i]));

		return lookupTable;
	}

	void TimeActLookupTable::destroy()
	{
		for (size_t i = 0; i < this->m_groupCount; i++)
		{
			if (this->m_groups[i])
				delete this->m_groups[i];
		}
	}

	void TimeActLookupTable::insertTimeAct(TimeActTrack* tae)
	{
		bool found = false;

		int taeID = tae->m_id;

		if (this->m_groups.size() == 0)
		{
			this->addGroup(tae);
			return;
		}

		//Look at the last inserted TAE
		if ((taeID - this->m_groups.back()->m_taeEndId) == 1)
		{
			this->m_groups.back()->m_taeEndId = taeID;
			found = true;
		}

		//If it's not the last, go look at the previous TAE groups created. This should never get executed, TAE list should be ordered by increasing TAE ID
		if (!found)
		{
			for (size_t i = 0; i < this->m_groupCount; i++)
			{
				if ((taeID - this->m_groups[i]->m_taeEndId) == 1)
				{
					this->m_groups[i]->m_taeEndId = taeID;
					found = true;
					break;
				}
			}
		}

		if (!found)
			this->addGroup(tae);
	}

	void TimeActLookupTable::addGroup(TimeActTrack* tae)
	{
		this->m_groupCount++;
		this->m_groups.push_back(TimeActTrackGroup::create(tae->m_id, tae->m_id, tae));

		std::sort(this->m_groups.begin(), this->m_groups.end(), compareTaeGroup);
	}

	int TimeActLookupTable::getMemoryRequirements()
	{
		return sizeof(TAE3::TimeActLookupTable) + this->m_groupCount * sizeof(TAE3::TimeActGroup);
	}

	TAE3::TimeActLookupTable TimeActLookupTable::generateBinary(RFile* file, ptrdiff_t tracksOffset)
	{
		int startPos = file->tell();

		TAE3::TimeActLookupTable lookupTable;
		lookupTable.groupCount = this->m_groupCount;
		lookupTable.groups = (TAE3::TimeActGroup*)(startPos + sizeof(TAE3::TimeActLookupTable));

		g_appLog->debugMessage(MsgLevel_Debug, "Writing TimeActLookupTable at %xh\n", file->tell());

		file->write(lookupTable);

		ptrdiff_t offset = tracksOffset;

		for (size_t i = 0; i < this->m_groupCount; i++)
		{
			g_appLog->debugMessage(MsgLevel_Debug, "Writing TimeActGroup %d (startId=%d, endId=%d, offset=%x) at %xh\n", i, this->m_groups[i]->m_taeStartId, this->m_groups[i]->m_taeEndId, tracksOffset + i * sizeof(TAE3::TimeActTrack), file->tell());

			this->m_groups[i]->generateBinary(file, offset);

			offset += sizeof(TimeActTrack) * (this->m_groups[i]->m_taeEndId - this->m_groups[i]->m_taeStartId + 1);
		}

		return lookupTable;
	}

	FileData* FileData::create(int fileId)
	{
		FileData* fileData = new FileData;
		fileData->m_fileID = fileId;
		fileData->m_numTimeActTracks = 0;

		fileData->m_lookupTable = TimeActLookupTable::create();

		return fileData;
	}

	FileData* FileData::createFromResource(TAE3::FileData* resource, TaeTemplate* taeTemplate)
	{
		FileData* fileData = new FileData;

		fileData->m_fileID = resource->fileID;
		fileData->m_numTimeActTracks = resource->numTimeActTracks;

		fileData->m_timeActTracks.reserve(resource->numTimeActTracks);

		for (size_t i = 0; i < resource->numTimeActTracks; i++)
			fileData->m_timeActTracks.push_back(TimeActTrack::createFromResource(&resource->timeActTracks[i], taeTemplate));

		fileData->m_lookupTable = TimeActLookupTable::create();

		return fileData;
	}

	void FileData::destroy()
	{
		for (size_t i = 0; i < this->m_numTimeActTracks; i++)
		{
			if (this->m_timeActTracks[i])
			{
				this->m_timeActTracks[i]->destroy();
				delete this->m_timeActTracks[i];
			}
		}

		this->m_timeActTracks.clear();

		if (this->m_lookupTable)
		{
			this->m_lookupTable->destroy();
			delete this->m_lookupTable;
		}
	}

	int FileData::getMemoryRequirements()
	{
		int size = sizeof(TAE3::FileData);

		for (size_t i = 0; i < this->m_numTimeActTracks; i++)
			size += this->m_timeActTracks[i]->getMemoryRequirements();

		size += this->m_lookupTable->getMemoryRequirements();

		for (size_t i = 0; i < this->m_numTimeActTracks; i++)
			size += this->m_timeActTracks[i]->m_timeActData->getMemoryRequirements();

		return size;
	}

	TAE3::FileData FileData::generateBinary(RFile* file, int animFileInfoMemReqs)
	{
		TAE3::FileData fileData;

		int startPos = file->tell();

		ptrdiff_t tracksOffset = startPos + sizeof(TAE3::FileData) + animFileInfoMemReqs;
		ptrdiff_t timeActDataOffset = startPos + sizeof(TAE3::FileData) + animFileInfoMemReqs + this->m_numTimeActTracks * sizeof(TAE3::TimeActTrack) + this->m_lookupTable->getMemoryRequirements();

		fileData.fileID = this->m_fileID;
		fileData.numTimeActTracks = this->m_numTimeActTracks;
		fileData.timeActTracks = (TAE3::TimeActTrack*)(tracksOffset);
		fileData.lookupTable = (TAE3::TimeActLookupTable*)(startPos + sizeof(TAE3::FileData) + animFileInfoMemReqs + this->m_numTimeActTracks * sizeof(TAE3::TimeActTrack));
		fileData.pVar68 = (int*)0xA0;
		fileData.numTimeActDatas = this->m_numTimeActTracks;
		fileData.timeActData = (TAE3::TimeActData*)(timeActDataOffset);

		g_appLog->debugMessage(MsgLevel_Debug, "Writing FileData at %xh\n", file->tell());

		file->write(fileData);

		file->seek((ptrdiff_t)fileData.timeActTracks);

		std::vector<TAE3::TimeActTrack> tae3Tracks;
		for (size_t trackIdx = 0; trackIdx < this->m_numTimeActTracks; trackIdx++)
		{
			g_appLog->debugMessage(MsgLevel_Debug, "Writing TimeActTrack %d (id=%d) at %xh\n", trackIdx, this->m_timeActTracks[trackIdx]->m_id, file->tell());

			tae3Tracks.push_back(this->m_timeActTracks[trackIdx]->generateBinary(file, (ptrdiff_t)fileData.timeActData + trackIdx * sizeof(TAE3::TimeActData)));
		}

		file->seek((ptrdiff_t)fileData.lookupTable);
		TAE3::TimeActLookupTable tae3LookupTable = this->m_lookupTable->generateBinary(file, (ptrdiff_t)fileData.timeActTracks);

		file->seek((ptrdiff_t)fileData.timeActData);

		std::vector<TAE3::TimeActData> tae3Datas;

		ptrdiff_t animDataOffset = file->tell() + this->m_numTimeActTracks * sizeof(TAE3::TimeActData);

		//Write TimeActDatas
		for (size_t trackIdx = 0; trackIdx < this->m_numTimeActTracks; trackIdx++)
		{
			g_appLog->debugMessage(MsgLevel_Debug, "Writing TimeActData %d (dataOffset=%x) at %xh\n", trackIdx, animDataOffset, file->tell());

			tae3Datas.push_back(this->m_timeActTracks[trackIdx]->m_timeActData->generateBinary(file, animDataOffset));

			animDataOffset += (this->m_timeActTracks[trackIdx]->m_timeActData->getMemoryRequirements() - sizeof(TAE3::TimeActData));
		}

		for (size_t trackIdx = 0; trackIdx < this->m_numTimeActTracks; trackIdx++)
		{
			TimeActTrack* track = this->m_timeActTracks[trackIdx];

			file->seek((ptrdiff_t)tae3Datas[trackIdx].animData);

			g_appLog->debugMessage(MsgLevel_Debug, "Writing AnimData %d at %xh\n", trackIdx, file->tell());

			TAE3::AnimData tae3AnimData = track->m_timeActData->m_animData->generateBinary(file);

			std::vector<float> tae3Times;
			//Write event times
			for (size_t eventGroupIdx = 0; eventGroupIdx < track->m_timeActData->m_eventGroupCount; eventGroupIdx++)
			{
				EventGroup* group = track->m_timeActData->m_eventGroups[eventGroupIdx];

				for (size_t eventIdx = 0; eventIdx < group->m_numEvents; eventIdx++)
				{
					TimeActEvent* event = group->m_events[eventIdx];

					g_appLog->debugMessage(MsgLevel_Debug, "Writing times %d, %d (start=%.3f, end=%.3f) at %xh\n", eventGroupIdx, eventIdx, event->m_startTime, event->m_endTime, file->tell());

					tae3Times.push_back(event->m_startTime);
					tae3Times.push_back(event->m_endTime);

					file->write(event->m_startTime);
					file->write(event->m_endTime);
				}
			}

			file->alignStream(16);

			int idx = 0;
			size_t eventDataSize = 0;
			std::vector<TAE3::TimeActEvent> tae3Events;
			//Write events
			for (size_t eventGroupIdx = 0; eventGroupIdx < track->m_timeActData->m_eventGroupCount; eventGroupIdx++)
			{
				EventGroup* group = track->m_timeActData->m_eventGroups[eventGroupIdx];

				for (size_t eventIdx = 0; eventIdx < group->m_numEvents; eventIdx++)
				{
					TimeActEvent* event = group->m_events[eventIdx];

					ptrdiff_t timesOffset = (ptrdiff_t)tae3Datas[trackIdx].times;

					ptrdiff_t startTimeOffset = timesOffset + idx * 8;
					ptrdiff_t endTimeOffset = timesOffset + sizeof(float) + idx * 8;

					ptrdiff_t eventDataOffset = (ptrdiff_t)tae3Datas[trackIdx].events + tae3Datas[trackIdx].eventCount * sizeof(TAE3::TimeActEvent) + eventDataSize;

					if (idx != 0)
						eventDataOffset = RMemory::align(eventDataOffset, 16);

					g_appLog->debugMessage(MsgLevel_Debug, "Writing TimeActEvent %d, %d (startOffset=%xh, endOffset=%xh, dataOffset=%xh) at %xh\n", eventGroupIdx, eventIdx, startTimeOffset, endTimeOffset, eventDataOffset, file->tell());

					tae3Events.push_back(event->generateBinary(file, startTimeOffset, endTimeOffset, eventDataOffset));

					eventDataSize += group->m_events[eventIdx]->m_eventData->getMemoryRequirements();

					idx++;
				}
			}

			idx = 0;
			std::vector<TAE3::EventData> tae3EventDatas;
			//Write event datas
			for (size_t eventGroupIdx = 0; eventGroupIdx < track->m_timeActData->m_eventGroupCount; eventGroupIdx++)
			{
				EventGroup* group = track->m_timeActData->m_eventGroups[eventGroupIdx];

				for (size_t eventIdx = 0; eventIdx < group->m_numEvents; eventIdx++)
				{
					TimeActEvent* event = group->m_events[eventIdx];

					file->seek((ptrdiff_t)tae3Events[idx].eventData);

					g_appLog->debugMessage(MsgLevel_Debug, "Writing TimeActEventData %d, %d at %xh\n", eventGroupIdx, eventIdx, file->tell());

					tae3EventDatas.push_back(event->m_eventData->generateBinary(file));

					idx++;
				}
			}

			file->seek((ptrdiff_t)tae3Datas[trackIdx].eventGroups);

			int pos = file->tell();
			int offset = 0;
			std::vector<TAE3::EventGroup> tae3Groups;
			//Write event groups
			for (size_t eventGroupIdx = 0; eventGroupIdx < track->m_timeActData->m_eventGroupCount; eventGroupIdx++)
			{
				EventGroup* group = track->m_timeActData->m_eventGroups[eventGroupIdx];

				ptrdiff_t groupDataOffset = pos + track->m_timeActData->m_eventGroupCount * sizeof(TAE3::EventGroup) + offset;

				g_appLog->debugMessage(MsgLevel_Debug, "Writing EventGroup %d (numEvents=%d, groupOffset=%xh) at %xh\n", eventGroupIdx, group->m_numEvents, groupDataOffset, file->tell());

				tae3Groups.push_back(group->generateBinary(file, groupDataOffset));

				offset += group->getMemoryRequirements() - sizeof(TAE3::EventGroup);
			}

			//Write event group datas
			for (size_t eventGroupIdx = 0; eventGroupIdx < track->m_timeActData->m_eventGroupCount; eventGroupIdx++)
			{
				EventGroup* group = track->m_timeActData->m_eventGroups[eventGroupIdx];

				g_appLog->debugMessage(MsgLevel_Debug, "Writing EventGroupData %d (type=%d) at %xh\n", eventGroupIdx, group->getGroupTypeID(), file->tell());

				file->seek((ptrdiff_t)tae3Groups[eventGroupIdx].groupData);

				TAE3::EventGroupData tae3GroupData = group->m_groupData->generateBinary(file);

				for (size_t eventIdx = 0; eventIdx < group->m_numEvents; eventIdx++)
				{
					ptrdiff_t eventOffset = findEventOffset((ptrdiff_t)tae3Datas[trackIdx].events, getEventList(track->m_timeActData), group->m_events[eventIdx]);

					g_appLog->debugMessage(MsgLevel_Debug, "Writing EventPtr %d, %d (ptr=%x) at %xh\n", eventGroupIdx, eventIdx, eventOffset, file->tell());

					file->write(eventOffset);
				}

				file->alignStream(16);
			}
		}

		return fileData;
	}

	AnimFileInfo* AnimFileInfo::create(int sibFileId, int skeletonFileId, std::wstring sibName, std::wstring skeletonName)
	{
		AnimFileInfo* animFileInfo = new AnimFileInfo;

		animFileInfo->m_skeletonFileId = skeletonFileId;
		animFileInfo->m_sibFileId = sibFileId;
		animFileInfo->m_skeletonName = skeletonName;
		animFileInfo->m_sibName = sibName;

		return animFileInfo;
	}

	AnimFileInfo* AnimFileInfo::createFromResource(TAE3::AnimFileInfo* resource)
	{
		AnimFileInfo* animFileInfo = new AnimFileInfo;

		animFileInfo->m_skeletonFileId = resource->skeletonFileId;
		animFileInfo->m_sibFileId = resource->sibFileId;
		animFileInfo->m_skeletonName = resource->skeletonName;
		animFileInfo->m_sibName = resource->sibName;

		return animFileInfo;
	}

	int AnimFileInfo::getMemoryRequirements()
	{
		if (this->m_sibName.compare(this->m_skeletonName) == 0)
			return RMemory::align(sizeof(TAE3::AnimFileInfo) + sizeof(wchar_t) * this->m_sibName.size(), 16);

		return RMemory::align(sizeof(TAE3::AnimFileInfo) + sizeof(wchar_t) * this->m_sibName.size() + sizeof(wchar_t) * this->m_skeletonName.size(), 16);
	}

	TAE3::AnimFileInfo AnimFileInfo::generateBinary(RFile* file)
	{
		TAE3::AnimFileInfo animFileInfo;

		file->alignStream(16);
		int startPos = file->tell();

		animFileInfo.iVar80 = 1;
		animFileInfo.pSkeletonFileID = (int*)(startPos + RMemory::getOffset(&animFileInfo.skeletonFileId, &animFileInfo));
		animFileInfo.skeletonFileId = this->m_skeletonFileId;
		animFileInfo.sibFileId = this->m_sibFileId;
		animFileInfo.fileData = (TAE3::FileData*)0x50;
		animFileInfo.iVarA0 = 0;
		animFileInfo.pNames = (const wchar_t**)(startPos + RMemory::getOffset(&animFileInfo.skeletonName, &animFileInfo));
		animFileInfo.skeletonName = (const wchar_t*)(startPos + sizeof(TAE3::AnimFileInfo));
		animFileInfo.sibName = (const wchar_t*)(startPos + sizeof(TAE3::AnimFileInfo));

		g_appLog->debugMessage(MsgLevel_Debug, "Writing AnimFileInfo at %xh\n", file->tell());

		file->write(animFileInfo);

		g_appLog->debugMessage(MsgLevel_Debug, "Writing skeletonName at %xh\n", file->tell());

		file->writeString(this->m_skeletonName);

		return animFileInfo;
	}

	TimeAct* TimeAct::create(int fileId, int sibFileId, int skeletonFileId, std::wstring sibName, std::wstring skeletonName, TaeTemplate* taeTemplate)
	{
		TimeAct* timeAct = new TimeAct;
		
		timeAct->m_fileData = FileData::create(fileId);
		timeAct->m_animFileInfo = AnimFileInfo::create(sibFileId, skeletonFileId, sibName, skeletonName);

		return timeAct;
	}

	TimeAct* TimeAct::createFromFile(std::wstring filepath, TaeTemplate* taeTemplate)
	{
		UINT64 size;
		void* buffer;

		UINT64 bytesRead = RFile::allocAndLoad(filepath, &buffer, &size);

		if (bytesRead > 0)
		{
			TAE3::TimeAct* tae3 = static_cast<TAE3::TimeAct*>(buffer);
			tae3->locate();

			TimeAct* timeAct = new TimeAct;

			timeAct->m_filePath = filepath;
			timeAct->m_name = std::filesystem::path(filepath).filename();

			timeAct->m_fileData = FileData::createFromResource(tae3->fileData, taeTemplate);
			timeAct->m_animFileInfo = AnimFileInfo::createFromResource(tae3->animFileInfo);

			timeAct->initialise();

			int fileSize = timeAct->getMemoryRequirements();

			g_appLog->debugMessage(MsgLevel_Debug, "Read %ws for %d bytes\n", filepath.c_str(), fileSize);

			return timeAct;
		}

		return nullptr;
	}

	TimeAct* TimeAct::createFromResource(TAE3::TimeAct* resource, TaeTemplate* taeTemplate)
	{
		TimeAct* timeAct = new TimeAct;

		timeAct->m_fileData = FileData::createFromResource(resource->fileData, taeTemplate);
		timeAct->m_animFileInfo = AnimFileInfo::createFromResource(resource->animFileInfo);

		timeAct->initialise();

		return timeAct;
	}

	void TimeAct::initialise()
	{
		this->buildLookupTable();

		this->m_initialised = true;
	}

	void TimeAct::destroy()
	{
		this->m_initialised = false;

		if (this->m_fileData)
		{
			this->m_fileData->destroy();
			delete this->m_fileData;
		}

		if (this->m_animFileInfo)
			delete this->m_animFileInfo;

		delete this;
	}

	TimeActTrack* TimeAct::timeActTrackLookup(int id)
	{
		int startIdx = 0;
		int endIdx = this->getNumTimeActTracks() - 1;

		while (startIdx <= endIdx)
		{
			int midIdx = (startIdx + endIdx) / 2;

			TimeActTrack* track = this->getTimeActTrack(midIdx);

			if (track->m_id < id)
				startIdx = midIdx + 1;
			else if (track->m_id > id)
				endIdx = midIdx - 1;
			else
				return track;
		}

		return nullptr;
	}

	TimeActTrack* TimeAct::addTimeActTrack(int id, int lenght, int fps)
	{
		int startIdx = 0;
		int endIdx = this->m_fileData->m_numTimeActTracks - 1;
		int insertIdx = -1;

		TimeActTrack* sameTrack = this->timeActTrackLookup(id);

		bool exists = (sameTrack != nullptr);

		if (!exists)
		{
			TimeActTrack* track = TimeActTrack::create(id, lenght, fps);

			this->m_fileData->m_numTimeActTracks++;
			this->m_fileData->m_timeActTracks.push_back(track);

			std::sort(this->m_fileData->m_timeActTracks.begin(), this->m_fileData->m_timeActTracks.end(), compareTracks);

			this->buildLookupTable();

			return track;
		}

		return sameTrack;
	}

	bool TimeAct::deleteTimeActTrack(int id)
	{
		int startIdx = 0;
		int endIdx = this->m_fileData->m_numTimeActTracks - 1;
		int deleteIdx = -1;

		while (startIdx <= endIdx)
		{
			int midIdx = (startIdx + endIdx) / 2;

			if (this->m_fileData->m_timeActTracks[midIdx]->m_id == id)
			{
				deleteIdx = midIdx;
				break;
			}

			if (this->m_fileData->m_timeActTracks[midIdx]->m_id > id)
				endIdx = midIdx - 1;
			else
				startIdx = midIdx + 1;
		}

		if (deleteIdx > -1)
		{
			this->m_fileData->m_numTimeActTracks--;
			this->m_fileData->m_timeActTracks.erase(this->m_fileData->m_timeActTracks.begin() + deleteIdx);

			this->buildLookupTable();

			return true;
		}

		return false;
	}

	void TimeAct::save(std::wstring filepath)
	{
		g_appLog->debugMessage(MsgLevel_Info, "Saving TimeAct at %ws\n", filepath.c_str());

		if (std::filesystem::exists(filepath))
		{
			std::filesystem::path bak_path = filepath;
			bak_path.replace_extension(L".tae.bak");

			std::filesystem::copy_file(filepath, bak_path, std::filesystem::copy_options::overwrite_existing);
		}

		RFile* fileRes = RFile::create(filepath);

		TAE3::TimeAct tae = this->generateBinary(fileRes);

		delete fileRes;
	}

	int TimeAct::getMemoryRequirements()
	{
		return sizeof(TAE3::TimeAct) + 16 + this->m_fileData->getMemoryRequirements() + this->m_animFileInfo->getMemoryRequirements();
	}

	//TODO: Finish this
	TAE3::TimeAct TimeAct::generateBinary(RFile* file)
	{
		TAE3::TimeAct timeAct;

		memcpy(timeAct.magic, "TAE ", 4);
		timeAct.bigEndian = false;
		timeAct.bVar5 = 0;
		timeAct.bVar6 = 0;
		timeAct.is64Bit = -1;
		timeAct.version = 0x1000C;
		timeAct.fileSize = this->getMemoryRequirements();
		timeAct.flags = (BYTE*)sizeof(TAE3::TimeAct);
		timeAct.isReadable = 1;
		timeAct.fileData = (TAE3::FileData*)((int)timeAct.flags + 16);
		timeAct.animFileInfo = (TAE3::AnimFileInfo*)((int)timeAct.fileData + sizeof(TAE3::FileData));
		timeAct.iVar30 = 1;
		timeAct.iVar38 = 0;

		g_appLog->debugMessage(MsgLevel_Debug, "Writing TimeAct at %xh\n", file->tell());

		file->write(timeAct);

		BYTE flags[16] = { 1, 0, 1, 2, 2, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0 };

		file->seek((ptrdiff_t)timeAct.flags);
		file->writeArray(flags, 16);

		file->seek((ptrdiff_t)timeAct.fileData);
		this->m_fileData->generateBinary(file, this->m_animFileInfo->getMemoryRequirements());

		file->seek((ptrdiff_t)timeAct.animFileInfo);
		this->m_animFileInfo->generateBinary(file);

		file->seek(timeAct.fileSize - 4);
		file->write(0);

		return timeAct;
	}

	void TimeAct::buildLookupTable()
	{
		TimeActLookupTable* lookupTable = this->m_fileData->m_lookupTable;
		std::vector<TimeActTrack*> timeActTracks = this->m_fileData->m_timeActTracks;

		lookupTable->m_groupCount = 0;
		lookupTable->m_groups.clear();

		for (size_t i = 0; i < timeActTracks.size(); i++)
			lookupTable->insertTimeAct(timeActTracks[i]);
	}
}