#pragma once
#include <Windows.h>

namespace TimeAct
{
	namespace TAE3
	{
		struct EventData
		{
			int type;
			BYTE* args;

			void locate(void* ptr);
		};

		struct TimeActEvent
		{
			float* startTime;
			float* endTime;
			EventData* eventData;

			void locate(void* ptr);
		};

		struct EventGroupData
		{
			int groupTypeID;
			size_t* offset;
			size_t* pVar10;
			size_t* pVar18;

			void locate(void* ptr);
		};

		struct EventGroup
		{
			int numEvents;
			TimeActEvent** events;
			EventGroupData* groupData;
			size_t isReadable;

			void locate(void* ptr);
		};

		struct AnimData
		{
			int reference;
			void* sectionStart;
			void* sectionEnd;
			BYTE bVar18;
			BYTE fps;
			BYTE bVar1A;
			BYTE bVar1B;
			int lenght;
			size_t iVar20;
			size_t iVar28;

			void locate(void* ptr);
		};

		struct TimeActData
		{
			TimeActEvent* events;
			EventGroup* eventGroups;
			float* times;
			AnimData* animData;

			int eventCount;
			int eventGroupCount;
			int timeCount;

			void locate(void* ptr);
		};

		struct TimeActTrack
		{
			int id;
			TimeActData* timeActData;

			void locate(void* ptr);
		};

		struct TimeActGroup
		{
			int taeStartId;
			int taeEndId;
			TimeActTrack* timeActTracks;

			void locate(void* ptr);
		};

		struct TimeActLookupTable
		{
			int groupCount = 0;
			TimeActGroup* groups;

			void locate(void* ptr);
		};

		struct FileData
		{
			int fileID;
			int numTimeActTracks;
			TimeActTrack* timeActTracks;
			TimeActLookupTable* lookupTable;
			int* pVar68;
			int numTimeActDatas;
			TimeActData* timeActData;

			void locate(void* ptr);
		};

		struct AnimFileInfo
		{
			int iVar80;
			int* pSkeletonFileID;
			int skeletonFileId;
			int sibFileId;
			FileData* fileData;
			int iVarA0;
			const wchar_t** pNames;
			const wchar_t* skeletonName;
			const wchar_t* sibName;

			void locate(char* ptr);
		};

		struct TimeAct
		{
			char magic[4];
			BYTE bigEndian;
			BYTE bVar5;
			BYTE bVar6;
			BYTE is64Bit;
			int version;
			int fileSize;
			BYTE* flags;
			size_t isReadable;
			FileData* fileData;
			AnimFileInfo* animFileInfo;
			size_t iVar30;
			size_t iVar38;

			void locate();
		};
	}
}