#include "TimeActEvent.h"
#include "RCore.h"
#include "extern.h"

namespace TimeAct
{
	EventData* EventData::create(int groupId, int id, TaeTemplate* taeTemplate)
	{
		EventData* data = new EventData;

		data->m_value = id;
		data->m_data = TimeActEventData::create(groupId, id, taeTemplate);

		return data;
	}

	EventData* EventData::createFromResource(TAE3::EventData* resource, int groupId, TaeTemplate* taeTemplate)
	{
		EventData* data = new EventData;

		data->m_value = resource->type;
		data->m_data = TimeActEventData::createFromResource((char*)resource->args, groupId, data->m_value, taeTemplate);

		return data;
	}

	void EventData::destroy()
	{
		if (this->m_data)
		{
			this->m_data->destroy();
			delete this->m_data;
		}
	}

	int EventData::getMemoryRequirements()
	{
		return sizeof(TAE3::EventData) + this->m_data->getMemoryRequirements();
	}

	TAE3::EventData EventData::generateBinary(RFile* file)
	{
		int startPos = file->tell();

		TAE3::EventData eventData;
		eventData.type = this->m_value;
		eventData.args = (BYTE*)(startPos + sizeof(TAE3::EventData));

		file->write(eventData);

		this->m_data->generateBinary(file);

		return eventData;
	}

	TimeActEvent* TimeActEvent::create(int groupId, float start, float end, int id, TaeTemplate* taeTemplate)
	{
		TimeActEvent* event = new TimeActEvent;

		event->m_startTime = start;
		event->m_endTime = end;
		event->m_eventData = EventData::create(groupId, id, taeTemplate);

		return event;
	}

	TimeActEvent* TimeActEvent::createFromResource(TAE3::TimeActEvent* resource, int groupId, TaeTemplate* taeTemplate)
	{
		TimeActEvent* event = new TimeActEvent;

		event->m_startTime = *resource->startTime;
		event->m_endTime = *resource->endTime;
		event->m_eventData = EventData::createFromResource(resource->eventData, groupId, taeTemplate);

		return event;
	}

	void TimeActEvent::destroy()
	{
		if (this->m_eventData)
		{
			this->m_eventData->destroy();
			delete this->m_eventData;
		}
	}

	int TimeActEvent::getMemoryRequirements()
	{
		return RMemory::align(sizeof(TAE3::TimeActEvent) + this->m_eventData->getMemoryRequirements(), 16);
	}

	TAE3::TimeActEvent TimeActEvent::generateBinary(RFile* file, ptrdiff_t startTime, ptrdiff_t endTime, ptrdiff_t eventDataOffset)
	{
		int startPos = file->tell();

		TAE3::TimeActEvent event;
		event.startTime = (float*)(startTime);
		event.endTime = (float*)(endTime);
		event.eventData = (TAE3::EventData*)(eventDataOffset);

		file->write(event);

		return event;
	}

	EventGroupData* EventGroupData::create(int type)
	{
		EventGroupData* groupData = new EventGroupData;

		groupData->m_eventType = type;

		return groupData;
	}

	EventGroupData* EventGroupData::createFromResource(TAE3::EventGroupData* resource)
	{
		EventGroupData* groupData = new EventGroupData;

		groupData->m_eventType = resource->groupTypeID;

		return groupData;
	}

	int EventGroupData::getMemoryRequirements()
	{
		return sizeof(TAE3::EventGroupData);
	}

	TAE3::EventGroupData EventGroupData::generateBinary(RFile* file)
	{
		int startPos = file->tell();

		TAE3::EventGroupData groupData;

		groupData.groupTypeID = this->m_eventType;
		groupData.offset = (size_t*)(startPos + RMemory::getOffset(&groupData.offset, &groupData.pVar10));
		groupData.pVar10 = 0;
		groupData.pVar18 = 0;

		file->write(groupData);

		return groupData;
	}

	bool EventGroup::lessThan(EventGroup* first, EventGroup* second)
	{
		return first->m_groupData->m_eventType < second->m_groupData->m_eventType;
	}

	EventGroup* EventGroup::create(int id)
	{
		EventGroup* eventGroup = new EventGroup;

		eventGroup->m_numEvents = 0;

		eventGroup->m_groupData = EventGroupData::create(id);

		return eventGroup;
	}

	EventGroup* EventGroup::createFromResource(TAE3::EventGroup* resource, TaeTemplate* taeTemplate)
	{
		EventGroup* eventGroup = new EventGroup;

		eventGroup->m_numEvents = resource->numEvents;

		eventGroup->m_events.reserve(resource->numEvents);

		for (size_t i = 0; i < resource->numEvents; i++)
			eventGroup->m_events.push_back(TimeActEvent::createFromResource(resource->events[i], resource->groupData->groupTypeID, taeTemplate));

		eventGroup->m_groupData = EventGroupData::createFromResource(resource->groupData);

		return eventGroup;
	}

	void EventGroup::destroy()
	{
		for (size_t i = 0; i < this->m_numEvents; i++)
		{
			if (this->m_events[i])
			{
				this->m_events[i]->destroy();
				delete this->m_events[i];
			}
		}

		this->m_events.clear();

		if (this->m_groupData)
			delete this->m_groupData;
	}

	TimeActEvent* EventGroup::addEvent(float startTime, float endTime, int value, TaeTemplate* taeTemplate)
	{
		TimeActEvent* event = TimeActEvent::create(this->m_groupData->m_eventType, startTime, endTime, value, taeTemplate);

		this->m_numEvents++;
		this->m_events.push_back(event);

		return event;
	}

	bool EventGroup::deleteEvent(int idx)
	{
		if (idx > this->m_numEvents)
			return false;

		this->m_numEvents--;

		TimeActEvent* event = this->m_events[idx];
		delete event;

		this->m_events.erase(this->m_events.begin() + idx);

		return true;
	}

	int EventGroup::getMemoryRequirements()
	{
		return sizeof(TAE3::EventGroup) + this->m_groupData->getMemoryRequirements() + RMemory::align(sizeof(UINT64) * this->m_numEvents, 16);
	}

	TAE3::EventGroup EventGroup::generateBinary(RFile* file, ptrdiff_t groupDataOffset)
	{
		file->alignStream(16);
		int startPos = file->tell();

		TAE3::EventGroup group;
		group.numEvents = this->m_numEvents;
		group.events = (TAE3::TimeActEvent**)(groupDataOffset + sizeof(TAE3::EventGroupData));
		group.groupData = (TAE3::EventGroupData*)(groupDataOffset);
		group.isReadable = 0;

		file->write(group);

		return group;
	}
}