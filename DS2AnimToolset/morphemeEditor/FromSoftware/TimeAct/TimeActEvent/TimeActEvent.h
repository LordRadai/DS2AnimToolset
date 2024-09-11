#include "../TimeActEvent/TimeActEventData/TimeActEventData.h"
#include "RCore.h"
#include "../TAE3/TAE3.h"

namespace TimeAct
{
	class EventData
	{
		friend class FileData;
		friend class TimeActEvent;
	public:
		EventData() {}
		~EventData() {}

		static EventData* create(int groupId, int id, TaeTemplate* taeTemplate);
		static EventData* createFromResource(TAE3::EventData* resource, int groupId, TaeTemplate* taeTemplate);

		void destroy();

		TimeActEventData* getData() const { return this->m_data; }
		int getValue() const { return this->m_value; }

		template <typename T> T getArgumentValue(int idx) const { return this->m_data->getArgumentValue(idx); }

	private:
		int getMemoryRequirements();
		TAE3::EventData generateBinary(RFile* file);

		int m_value = 0;
		TimeActEventData* m_data = nullptr;
	};

	class TimeActEvent
	{
		friend class FileData;
		friend class TimeActData;
		friend class EventGroup;
	public:
		TimeActEvent() {}
		~TimeActEvent() {}

		static TimeActEvent* create(int groupId, float start, float end, int id, TaeTemplate* taeTemplate);
		static TimeActEvent* createFromResource(TAE3::TimeActEvent* resource, int groupId, TaeTemplate* taeTemplate);

		void destroy();

		float getStartTime() const { return this->m_startTime; }
		float getEndTime() const { return this->m_endTime; }
		int getEventValue() const { return this->m_eventData->getValue(); }
		TimeActEventData* getData() const { return this->m_eventData->getData(); }
		int getNumArguments() const { return this->m_eventData->m_data->getNumArguments(); }
		Argument* getArgument(int idx) const { return this->m_eventData->m_data->getArgument(idx); }
		int getArgumentsSize(int idx) const { return this->m_eventData->m_data->getSize(); }
		template <typename T> T getArgumentValue(int idx) const { return this->m_eventData->getArgumentValue(idx); }

		void setStartTime(float time) { this->m_startTime = time; }
		void setEndTime(float time) { this->m_endTime = time; }
		void setValue(int value) { this->m_eventData->m_value = value; }

	protected:
		int getMemoryRequirements();
		TAE3::TimeActEvent generateBinary(RFile* file, ptrdiff_t startTime, ptrdiff_t endTime, ptrdiff_t eventDataOffset);

		float m_startTime = 0.f;
		float m_endTime = 0.f;
		EventData* m_eventData = nullptr;
	};

	class EventGroupData
	{
		friend class FileData;
		friend class EventGroup;

	public:
		EventGroupData() {}
		~EventGroupData() {}

		static EventGroupData* create(int type);
		static EventGroupData* createFromResource(TAE3::EventGroupData* resource);

		int getEventTypeID() const { return this->m_eventType; }
		void setEventTypeId(int id) { this->m_eventType = id; }

	protected:
		int getMemoryRequirements();
		TAE3::EventGroupData generateBinary(RFile* file);

		int m_eventType = -1;
	};

	class EventGroup
	{
		friend class FileData;
		friend class EventGroup;
		friend class TimeActData;
	public:
		EventGroup() {}
		~EventGroup() {}

		inline bool operator <(const EventGroup& other)
		{
			return (this->m_groupData->m_eventType < other.m_groupData->m_eventType);
		}

		static bool lessThan(EventGroup* first, EventGroup* second);

		static EventGroup* create(int id);
		static EventGroup* createFromResource(TAE3::EventGroup* resource, TaeTemplate* taeTemplate);

		void destroy();

		int getNumEvents() const { return this->m_numEvents; }
		int getGroupTypeID() const { return this->m_groupData->getEventTypeID(); }
		void setGroupTypeID(int id) { this->m_groupData->setEventTypeId(id); }

		TimeActEvent* getEvent(int idx) const { return this->m_events[idx]; }

		TimeActEvent* addEvent(float startTime, float endTime, int value, TaeTemplate* taeTemplate);
		bool deleteEvent(int idx);
	protected:
		int getMemoryRequirements();
		TAE3::EventGroup generateBinary(RFile* file, ptrdiff_t groupDataOffset);

		int m_numEvents = 0;
		std::vector<TimeActEvent*> m_events;
		EventGroupData* m_groupData = nullptr;
	};
}