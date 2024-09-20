#pragma once
#include <vector>
#include "RCore.h"
#include "../TimeAct.h"

namespace TimeAct
{
	namespace TaeExport
	{
		class TimeActTrackListExportXML;
		class TimeActArgumentExportXML;
		class TimeActEventExportXML;
		class TimeActGroupExportXML;
		class TimeActTrackExportXML;
		class TimeActTrackListExportXML;
		class TimeActExportXML;

		class TimeActArgumentExportXML
		{
			friend class TimeActEventExportXML;

		public:
			TimeActEventExportXML* getOwner() const { return this->m_owner; }
			std::string getName();
			DataType getType();

			void setAsBool(bool value);
			void setAsByte(INT8 value);
			void setAsUByte(UINT8 value);
			void setAsShort(INT16 value);
			void setAsUShort(UINT16 value);
			void setAsInt(INT32 value);
			void setAsUInt(UINT32 value);
			void setAsInt64(INT64 value);
			void setAsUInt64(UINT64 value);
			void setAsFloat(float value);

			bool getAsBool();
			INT8 getAsByte();
			UINT8 getAsUByte();
			INT16 getAsShort();
			UINT16 getAsUShort();
			INT32 getAsInt();
			UINT32 getAsUInt();
			INT64 getAsInt64();
			UINT64 getAsUInt64();
			float getAsFloat();

			std::string getAsString();

		protected:
			TimeActArgumentExportXML() {}
			~TimeActArgumentExportXML() {}

			static TimeActArgumentExportXML* create(TimeActEventExportXML* owner, tinyxml2::XMLElement* parent, std::string name);

			TimeActEventExportXML* m_owner = nullptr;
			tinyxml2::XMLElement* m_xmlElement = nullptr;
		};

		class TimeActEventExportXML
		{
			friend class TimeActGroupExportXML;

		public:
			float getStartTime();
			float getEndTime();
			int getEventId();
			TimeActArgumentExportXML* getArgument(int idx) const { return this->m_arguments[idx]; }
			TimeActGroupExportXML* getOwner() const { return this->m_owner; }

			int getNumArguments() const { return this->m_arguments.size(); }

			std::string getArgumentsString();

			TimeActArgumentExportXML* addArgument(std::string name);
			void deleteArgument(int idx);
			void clear();
		protected:
			TimeActEventExportXML() {}
			~TimeActEventExportXML() {}

			static TimeActEventExportXML* create(TimeActGroupExportXML* owner, tinyxml2::XMLElement* parent, int eventId, float startTime, float endTime);

			TimeActGroupExportXML* m_owner = nullptr;
			std::vector<TimeActArgumentExportXML*> m_arguments;
			tinyxml2::XMLElement* m_xmlElement = nullptr;
		};

		class TimeActGroupExportXML
		{
			friend class TimeActTrackExportXML;

		public:
			int getGroupId();
			int getNumEvents();
			TimeActEventExportXML* getEvent(int idx) const { return this->m_events[idx]; }
			TimeActTrackExportXML* getOwner() const { return this->m_owner; }

			TimeActEventExportXML* addEvent(float startTime, float endTime, int eventId);
			void deleteEvent(int idx);

			void clear();
		protected:
			TimeActGroupExportXML() {}
			~TimeActGroupExportXML() {}

			static TimeActGroupExportXML* create(TimeActTrackExportXML* owner, tinyxml2::XMLElement* parent, int groupId);

			TimeActTrackExportXML* m_owner = nullptr;
			std::vector<TimeActEventExportXML*> m_events;
			tinyxml2::XMLElement* m_xmlElement = nullptr;
		};

		class TimeActTrackExportXML
		{
			friend class TimeActTrackListExportXML;

		public:
			int getNumEventGroups();
			int getTrackId();
			int getFps();
			int getFrameCount();
			TimeActGroupExportXML* getEventGroup(int idx) const { return this->m_eventGroups[idx]; }
			TimeActTrackListExportXML* getOwner() const { return this->m_owner; }

			TimeActGroupExportXML* addEventGroup(int groupId);

			void deleteGroup(int idx);
			void clear();
		protected:
			TimeActTrackExportXML() {}
			~TimeActTrackExportXML() {}

			static TimeActTrackExportXML* create(TimeActTrackListExportXML* owner, tinyxml2::XMLElement* parent, int id, int fps, int numFrames);

			TimeActTrackListExportXML* m_owner = nullptr;
			std::vector<TimeActGroupExportXML*> m_eventGroups;
			tinyxml2::XMLElement* m_xmlElement = nullptr;
		};

		class TimeActTrackListExportXML
		{
			friend class TimeActExportXML;

		public:
			int getNumTracks();
			TimeActTrackExportXML* getTrack(int idx) const { return this->m_tracks[idx]; }
			TimeActExportXML* getOwner() const { return this->m_owner; }

			TimeActTrackExportXML* timeActTrackLookup(int id);

			TimeActTrackExportXML* addTrack(int id, int fps, int numFrames);
			void deleteTrack(int idx);
		protected:
			TimeActTrackListExportXML() {}
			~TimeActTrackListExportXML() {}

			static TimeActTrackListExportXML* create(TimeActExportXML* owner, tinyxml2::XMLElement* parent);

			TimeActExportXML* m_owner = nullptr;
			std::vector<TimeActTrackExportXML*> m_tracks;
			tinyxml2::XMLElement* m_xmlElement = nullptr;
		};

		class TimeActExportXML
		{
		public:
			static TimeActExportXML* create(std::string filePath, std::string name, int fileId);

			std::string getDstFileName() const { return this->m_dstFileName; }
			TimeActTrackListExportXML* getTrackList() const { return this->m_trackList; }

			std::string getName();
			int getFileId();
			int getSibFileId();
			std::string getSibName();
			int getSkeletonId();
			std::string getSkeletonName();

			void setDstFileName(std::string filepath) { this->m_dstFileName = filepath; }

			void setSibFile(int fileId, std::string name);
			void setSkeletonFile(int fileId, std::string name);

			TimeActTrackListExportXML* createTrackList();

			void findEventsWithId(std::vector<TimeActEventExportXML*>& buffer, const int id);

			bool save();

			void destroy();
		protected:
			TimeActExportXML() {}
			~TimeActExportXML() {}

			TimeActTrackListExportXML* m_trackList = nullptr;
			std::string m_dstFileName = "";

			tinyxml2::XMLDocument* m_xmlDoc = nullptr;
			tinyxml2::XMLElement* m_xmlElement = nullptr;
		};
	}
}