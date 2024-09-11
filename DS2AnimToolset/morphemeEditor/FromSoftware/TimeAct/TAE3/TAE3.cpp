#include <filesystem>
#include <assert.h>
#include "TAE3.h"
#include "framework.h"
#include "extern.h"
#include "RCore.h"

namespace TimeAct
{
	namespace TAE3
	{
		void TimeActEvent::locate(void* ptr)
		{
			RMemory::fixPtr(this->startTime, ptr);
			RMemory::fixPtr(this->endTime, ptr);
			RMemory::fixPtr(this->eventData, ptr);

			this->eventData->locate(ptr);
		}

		void EventGroupData::locate(void* ptr)
		{
			RMemory::fixPtr(this->offset, ptr);
		}

		void EventGroup::locate(void* ptr)
		{
			RMemory::fixPtr(this->events, ptr);
			RMemory::fixPtr(this->groupData, ptr);

			for (size_t i = 0; i < this->numEvents; i++)
				RMemory::fixPtr(this->events[i], ptr);

			this->groupData->locate(ptr);
		}

		void EventData::locate(void* ptr)
		{
			RMemory::fixPtr(this->args, ptr);
		}

		void AnimData::locate(void* ptr)
		{
			RMemory::fixPtr(this->sectionStart, ptr);
			RMemory::fixPtr(this->sectionEnd, ptr);
		}

		void TimeActData::locate(void* ptr)
		{
			RMemory::fixPtr(this->events, ptr);
			RMemory::fixPtr(this->eventGroups, ptr);
			RMemory::fixPtr(this->times, ptr);
			RMemory::fixPtr(this->animData, ptr);

			this->animData->locate(ptr);

			for (size_t i = 0; i < this->eventCount; i++)
				this->events[i].locate(ptr);

			for (size_t i = 0; i < this->eventGroupCount; i++)
				this->eventGroups[i].locate(ptr);
		}

		void TimeActTrack::locate(void* ptr)
		{
			RMemory::fixPtr(this->timeActData, ptr);
		}

		void TimeActGroup::locate(void* ptr)
		{
			RMemory::fixPtr(this->timeActTracks, ptr);
		}

		void TimeActLookupTable::locate(void* ptr)
		{
			RMemory::fixPtr(this->groups, ptr);

			for (size_t i = 0; i < this->groupCount; i++)
				this->groups[i].locate(ptr);
		}

		void FileData::locate(void* ptr)
		{
			RMemory::fixPtr(this->lookupTable, ptr);
			RMemory::fixPtr(this->timeActData, ptr);
			RMemory::fixPtr(this->timeActTracks, ptr);
			RMemory::fixPtr(this->pVar68, ptr);

			this->lookupTable->locate(ptr);

			for (size_t i = 0; i < this->numTimeActTracks; i++)
				this->timeActTracks[i].locate(ptr);

			for (size_t i = 0; i < this->numTimeActDatas; i++)
				this->timeActData[i].locate(ptr);
		}

		void AnimFileInfo::locate(char* ptr)
		{
			RMemory::fixPtr(this->pSkeletonFileID, ptr);
			RMemory::fixPtr(this->fileData, ptr);
			RMemory::fixPtr(this->pNames, ptr);
			RMemory::fixPtr(this->skeletonName, ptr);
			RMemory::fixPtr(this->sibName, ptr);
		}

		void TimeAct::locate()
		{
			char* ptr = (char*)this;

			RMemory::fixPtr(this->flags, ptr);
			RMemory::fixPtr(this->fileData, ptr);
			RMemory::fixPtr(this->animFileInfo, ptr);

			this->fileData->locate(ptr);
			this->animFileInfo->locate(ptr);
		}
	}
}