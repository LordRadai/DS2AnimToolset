#pragma once
#include "TimeActEvent/TimeActEvent.h"
#include "TAE3/TAE3.h"

namespace TimeAct
{
	class AnimData
	{
		friend class FileData;
		friend class TimeActData;
		friend class TimeActTrack;

	public:
		AnimData() {}
		~AnimData() {}

		static AnimData* create(int lenght, int fps);
		static AnimData* createFromResource(TAE3::AnimData* resource);

	protected:
		int getMemoryRequirements();
		TAE3::AnimData generateBinary(RFile* file);

		int m_reference = 0;
		BYTE m_fps = 0;
		int m_lenght = 0;
	};

	class TimeActData
	{
		friend class TimeActTrack;
		friend class FileData;

	public:
		TimeActData() {}
		~TimeActData() {}

		static TimeActData* create(int lenght, int fps);
		static TimeActData* createFromResource(TAE3::TimeActData* resource, TaeTemplate* taeTemplate);

		void destroy();

		int getNumEventGroups() const { return this->m_eventGroupCount; }
		EventGroup* getEventGroup(int idx) const { return this->m_eventGroups[idx]; }
		int getLenght() const { return this->m_animData->m_lenght; }
		int getFps() const { return this->m_animData->m_fps; }

	protected:
		int getMemoryRequirements();
		TAE3::TimeActData generateBinary(RFile* file, ptrdiff_t animDataOffset);

		std::vector<EventGroup*> m_eventGroups;
		AnimData* m_animData = nullptr;
		int m_eventGroupCount = 0;
	};

	class TimeActTrack
	{
		friend class TimeAct;
		friend class FileData;
		friend class TimeActLookupTable;

	public:
		TimeActTrack() {}
		~TimeActTrack() {}

		inline bool operator <(const TimeActTrack& other)
		{
			return this->m_id < other.m_id;
		}

		static TimeActTrack* create(int id, int lenght, int fps);
		static TimeActTrack* createFromResource(TAE3::TimeActTrack* resource, TaeTemplate* taeTemplate);

		void destroy();

		int getId() const { return this->m_id; }
		int getNumEventGroups() const { return this->m_timeActData->getNumEventGroups(); }
		EventGroup* getEventGroup(int idx) const { return this->m_timeActData->getEventGroup(idx); }
		int getLenght() const { return this->m_timeActData->getLenght(); }
		int getFps() const { return this->m_timeActData->getFps(); }

		EventGroup* addGroup(int groupTypeId);
		bool deleteGroup(int idx);

		TimeActEvent* addEvent(int groupIdx, float startTime, float endTime, int value, TaeTemplate* taeTemplate);
		bool deleteEvent(int groupIdx, int eventIdx);
	protected:
		int getMemoryRequirements();
		TAE3::TimeActTrack generateBinary(RFile* file, ptrdiff_t taeDataOffset);

		int m_id = 0;
		TimeActData* m_timeActData = nullptr;
	};

	class TimeActTrackGroup
	{
		friend class FileData;
		friend class TimeActLookupTable;
		friend class TimeAct;

	public:
		TimeActTrackGroup() {}
		~TimeActTrackGroup() {}

		inline bool operator <(const TimeActTrackGroup& other)
		{
			return this->m_taeStartId < other.m_taeStartId;
		}

		static TimeActTrackGroup* create(int startTae, int endTae, TimeActTrack* source);
		static TimeActTrackGroup* createFromResource(TAE3::TimeActGroup* resource);

		int getStartId() const { return this->m_taeStartId; }
		int getEndId() const { return this->m_taeEndId; }
	protected:
		int getMemoryRequirements();
		TAE3::TimeActGroup generateBinary(RFile* file, ptrdiff_t offset);

		int m_taeStartId = 0;
		int m_taeEndId = 0;
		TimeActTrack* m_tae;
	};

	class TimeActLookupTable
	{
		friend class FileData;
		friend class TimeAct;

	public:
		TimeActLookupTable() {}
		~TimeActLookupTable() {}

		static TimeActLookupTable* create();
		static TimeActLookupTable* createFromResource(TAE3::TimeActLookupTable* resource);
		void destroy();

		void insertTimeAct(TimeActTrack* tae);
		void addGroup(TimeActTrack* tae);

	protected:
		int getMemoryRequirements();
		TAE3::TimeActLookupTable generateBinary(RFile* file, ptrdiff_t tracksOffset);

		int m_groupCount = 0;
		std::vector<TimeActTrackGroup*> m_groups;
	};

	class FileData
	{
		friend class TimeAct;

	public:
		FileData() {}
		~FileData() {}

		static FileData* create(int fileId);
		static FileData* createFromResource(TAE3::FileData* resource, TaeTemplate* taeTemplate);
		void destroy();

	protected:
		int getMemoryRequirements();
		TAE3::FileData generateBinary(RFile* file, int animFileInfoMemReqs);

		int m_fileID = 0;
		int m_numTimeActTracks = 0;
		std::vector<TimeActTrack*> m_timeActTracks;
		TimeActLookupTable* m_lookupTable = nullptr;
	};

	class AnimFileInfo
	{
		friend class TimeAct;
	public:
		AnimFileInfo() {}
		~AnimFileInfo() {}

		static AnimFileInfo* create(int sibFileId, int skeletonFileId, std::wstring sibName, std::wstring skeletonName);
		static AnimFileInfo* createFromResource(TAE3::AnimFileInfo* resource);

	protected:
		int getMemoryRequirements();
		TAE3::AnimFileInfo generateBinary(RFile* file);

		int m_skeletonFileId = 0;
		int m_sibFileId = 0;
		std::wstring m_skeletonName;
		std::wstring m_sibName;
	};

	class TimeAct
	{
	public:
		static TimeAct* create(int fileId, int sibFileId, int skeletonFileId, std::wstring sibName, std::wstring skeletonName, TaeTemplate* taeTemplate);
		static TimeAct* createFromFile(std::wstring filepath, TaeTemplate* taeTemplate);
		static TimeAct* createFromResource(TAE3::TimeAct* resource, TaeTemplate* taeTemplate);

		void initialise();
		void destroy();

		bool isInitialised() const { return this->m_initialised; }
		std::wstring getName() const { return this->m_name; }
		std::wstring getFilePath() const { return this->m_filePath; }
		int getFileId() const { return this->m_fileData->m_fileID; }
		int getNumTimeActTracks() const { return this->m_fileData->m_numTimeActTracks; }
		TimeActTrack* getTimeActTrack(int idx) const { return this->m_fileData->m_timeActTracks[idx]; }
		int getSkeletonFileId() const { return this->m_animFileInfo->m_skeletonFileId; }
		int getSibFileId() const { return this->m_animFileInfo->m_sibFileId; }
		std::wstring getSkeletonName() const { return this->m_animFileInfo->m_skeletonName; }
		std::wstring getSibName() const { return this->m_animFileInfo->m_sibName; }

		void setFileId(int id) { this->m_fileData->m_fileID = id; }
		void setSibFileId(int id) { this->m_animFileInfo->m_sibFileId = id; }
		void setSkeletonFileId(int id) { this->m_animFileInfo->m_skeletonFileId = id; }
		void setSibName(std::wstring name) { this->m_animFileInfo->m_sibName = name; }
		void setSkeletonName(std::wstring name) { this->m_animFileInfo->m_skeletonName = name; }

		TimeActTrack* timeActTrackLookup(int id);

		TimeActTrack* addTimeActTrack(int id, int lenght, int fps);
		bool deleteTimeActTrack(int id);

		void save(std::wstring filepath);

	protected:
		TimeAct() {}
		~TimeAct() {}

		void buildLookupTable();

		int getMemoryRequirements();
		TAE3::TimeAct generateBinary(RFile* file);

		std::wstring m_filePath = L"";
		std::wstring m_name = L"";
		bool m_initialised = false;
		FileData* m_fileData = nullptr;
		AnimFileInfo* m_animFileInfo = nullptr;
	};
}