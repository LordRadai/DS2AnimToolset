#include "TaeExport.h"
#include "RCore.h"
#include "extern.h"

namespace TimeAct
{
	namespace TaeExport
	{
		namespace
		{
			bool isTrackLessThan(TimeActTrackExportXML* first, TimeActTrackExportXML* second)
			{
				return first->getTrackId() < second->getTrackId();
			}
		}

		TimeActArgumentExportXML* TimeActArgumentExportXML::create(TimeActArgumentListExportXML* owner, tinyxml2::XMLElement* parent, std::string name)
		{
			TimeActArgumentExportXML* argExport = new TimeActArgumentExportXML;
			argExport->m_owner = owner;
			argExport->m_xmlElement = parent->InsertNewChildElement("Argument");
			argExport->m_xmlElement->SetAttribute("name", name.c_str());
			argExport->m_xmlElement->SetAttribute("type", "invalid");

			return argExport;
		}

		std::string TimeActArgumentExportXML::getName()
		{
			return this->m_xmlElement->FindAttribute("name")->Value();
		}

		DataType TimeActArgumentExportXML::getType()
		{
			std::string type = this->m_xmlElement->FindAttribute("type")->Value();

			if (type.compare("bool") == 0)
				return kBool;
			else if (type.compare("byte") == 0)
				return kByte;
			else if (type.compare("ubyte") == 0)
				return kUByte;
			else if (type.compare("short") == 0)
				return kShort;
			else if (type.compare("ushort") == 0)
				return kUShort;
			else if (type.compare("int") == 0)
				return kInt;
			else if (type.compare("uint") == 0)
				return kUInt;
			else if (type.compare("float") == 0)
				return kFloat;
			else if (type.compare("int64") == 0)
				return kInt64;
			else if (type.compare("uint64") == 0)
				return kUInt64;
			else
				return kTypeInvalid;
		}

		void TimeActArgumentExportXML::setAsBool(bool value)
		{
			this->m_xmlElement->SetAttribute("type", "bool");
			this->m_xmlElement->SetText(value);
		}

		void TimeActArgumentExportXML::setAsByte(INT8 value)
		{
			this->m_xmlElement->SetAttribute("type", "byte");
			this->m_xmlElement->SetText(value);
		}

		void TimeActArgumentExportXML::setAsUByte(UINT8 value)
		{
			this->m_xmlElement->SetAttribute("type", "ubyte");
			this->m_xmlElement->SetText(value);
		}

		void TimeActArgumentExportXML::setAsShort(INT16 value)
		{
			this->m_xmlElement->SetAttribute("type", "short");
			this->m_xmlElement->SetText(value);
		}

		void TimeActArgumentExportXML::setAsUShort(UINT16 value)
		{
			this->m_xmlElement->SetAttribute("type", "ushort");
			this->m_xmlElement->SetText(value);
		}

		void TimeActArgumentExportXML::setAsInt(INT32 value)
		{
			this->m_xmlElement->SetAttribute("type", "int");
			this->m_xmlElement->SetText(value);
		}

		void TimeActArgumentExportXML::setAsUInt(UINT32 value)
		{
			this->m_xmlElement->SetAttribute("type", "uint");
			this->m_xmlElement->SetText(value);
		}

		void TimeActArgumentExportXML::setAsInt64(INT64 value)
		{
			this->m_xmlElement->SetAttribute("type", "int64");
			this->m_xmlElement->SetText(value);
		}

		void TimeActArgumentExportXML::setAsUInt64(UINT64 value)
		{
			this->m_xmlElement->SetAttribute("type", "uint64");
			this->m_xmlElement->SetText(value);
		}

		void TimeActArgumentExportXML::setAsFloat(float value)
		{
			this->m_xmlElement->SetAttribute("type", "float");
			this->m_xmlElement->SetText(value);
		}

		bool TimeActArgumentExportXML::getAsBool()
		{
			std::string value = this->m_xmlElement->GetText();

			if (value.compare("true") == 0)
				return true;

			return false;
		}

		INT8 TimeActArgumentExportXML::getAsByte()
		{
			return std::stoi(this->m_xmlElement->GetText());
		}

		UINT8 TimeActArgumentExportXML::getAsUByte()
		{
			return std::stoi(this->m_xmlElement->GetText());
		}

		INT16 TimeActArgumentExportXML::getAsShort()
		{
			return std::stoi(this->m_xmlElement->GetText());
		}

		UINT16 TimeActArgumentExportXML::getAsUShort()
		{
			return std::stoi(this->m_xmlElement->GetText());
		}

		INT32 TimeActArgumentExportXML::getAsInt()
		{
			return std::stoi(this->m_xmlElement->GetText());
		}

		UINT32 TimeActArgumentExportXML::getAsUInt()
		{
			return std::stoi(this->m_xmlElement->GetText());
		}

		INT64 TimeActArgumentExportXML::getAsInt64()
		{
			return std::stoll(this->m_xmlElement->GetText());
		}

		UINT64 TimeActArgumentExportXML::getAsUInt64()
		{
			return std::stoull(this->m_xmlElement->GetText());
		}

		float TimeActArgumentExportXML::getAsFloat()
		{
			return std::stof(this->m_xmlElement->GetText());
		}

		TimeActArgumentListExportXML* TimeActArgumentListExportXML::create(TimeActEventExportXML* owner, tinyxml2::XMLElement* parent)
		{
			TimeActArgumentListExportXML* argList = new TimeActArgumentListExportXML;
			argList->m_owner = owner;
			argList->m_xmlElement = parent->InsertNewChildElement("ArgumentList");
			argList->m_xmlElement->SetAttribute("numArguments", 0);

			return argList;
		}

		int TimeActArgumentListExportXML::getNumArguments()
		{
			return this->m_xmlElement->FindAttribute("numArguments")->IntValue();
		}

		TimeActArgumentExportXML* TimeActArgumentListExportXML::addArgument(std::string name)
		{
			int numArgs = this->getNumArguments();
			this->m_xmlElement->SetAttribute("numArguments", ++numArgs);

			TimeActArgumentExportXML* argExport = TimeActArgumentExportXML::create(this, this->m_xmlElement, name);
			this->m_arguments.push_back(argExport);

			return argExport;
		}

		void TimeActArgumentListExportXML::deleteArgument(int idx)
		{
			this->m_arguments[idx]->m_xmlElement->DeleteChildren();
			this->m_xmlElement->DeleteChild(this->m_arguments[idx]->m_xmlElement);

			int numArgs = this->getNumArguments();
			this->m_xmlElement->SetAttribute("numArguments", --numArgs);
		}

		void TimeActArgumentListExportXML::clear()
		{
			int numArgs = this->getNumArguments();

			while (numArgs > 0)
			{
				this->deleteArgument(numArgs - 1);

				numArgs--;
			}
		}

		TimeActEventExportXML* TimeActEventExportXML::create(TimeActGroupExportXML* owner, tinyxml2::XMLElement* parent, int eventId, float startTime, float endTime)
		{
			TimeActEventExportXML* eventExport = new TimeActEventExportXML;
			eventExport->m_owner = owner;
			eventExport->m_xmlElement = parent->InsertNewChildElement("Event");

			RXML::createFloatElement(eventExport->m_xmlElement, "startTime", startTime);
			RXML::createFloatElement(eventExport->m_xmlElement, "endTime", endTime);
			RXML::createIntElement(eventExport->m_xmlElement, "EventID", eventId);

			return eventExport;
		}

		float TimeActEventExportXML::getStartTime()
		{
			return RXML::getFloatElement(this->m_xmlElement, "startTime");
		}

		float TimeActEventExportXML::getEndTime()
		{
			return RXML::getFloatElement(this->m_xmlElement, "endTime");
		}

		int TimeActEventExportXML::getEventId()
		{
			return RXML::getIntElement(this->m_xmlElement, "EventID");
		}

		TimeActArgumentListExportXML* TimeActEventExportXML::createArgumentList()
		{
			this->m_argumentList = TimeActArgumentListExportXML::create(this, this->m_xmlElement);

			return this->m_argumentList;
		}

		void TimeActEventExportXML::clear()
		{
			this->m_argumentList->clear();
			
			this->m_xmlElement->DeleteChildren();
		}

		TimeActGroupExportXML* TimeActGroupExportXML::create(TimeActTrackExportXML* owner, tinyxml2::XMLElement* parent, int groupId)
		{
			TimeActGroupExportXML* groupExport = new TimeActGroupExportXML;
			groupExport->m_owner = owner;
			groupExport->m_xmlElement = parent->InsertNewChildElement("EventGroup");
			groupExport->m_xmlElement->SetAttribute("numEvents", 0);

			RXML::createIntElement(groupExport->m_xmlElement, "GroupID", groupId);

			return groupExport;
		}

		int TimeActGroupExportXML::getGroupId()
		{
			return RXML::getIntElement(this->m_xmlElement, "GroupID");
		}

		int TimeActGroupExportXML::getNumEvents()
		{
			return this->m_xmlElement->FindAttribute("numEvents")->IntValue();
		}

		TimeActEventExportXML* TimeActGroupExportXML::addEvent(float startTime, float endTime, int eventId)
		{
			int eventCount = this->getNumEvents();
			this->m_xmlElement->SetAttribute("numEvents", ++eventCount);

			TimeActEventExportXML* eventExport = TimeActEventExportXML::create(this, this->m_xmlElement, eventId, startTime, endTime);
			this->m_events.push_back(eventExport);

			return eventExport;
		}

		void TimeActGroupExportXML::deleteEvent(int idx)
		{
			this->m_events[idx]->clear();
			this->m_xmlElement->DeleteChild(this->getEvent(idx)->m_xmlElement);
			this->m_events.erase(this->m_events.begin() + idx);

			int numEvents = this->getNumEvents();
			this->m_xmlElement->SetAttribute("numEvents", --numEvents);
		}

		void TimeActGroupExportXML::clear()
		{
			int numEvents = this->getNumEvents();

			while (numEvents > 0)
			{
				this->deleteEvent(numEvents - 1);

				numEvents--;
			}
		}

		TimeActTrackExportXML* TimeActTrackExportXML::create(TimeActTrackListExportXML* owner, tinyxml2::XMLElement* parent, int id, int fps, int numFrames)
		{
			TimeActTrackExportXML* trackExport = new TimeActTrackExportXML;
			trackExport->m_owner = owner;

			int startIdx = 0;
			int endIdx = parent->ChildElementCount() - 1;

			tinyxml2::XMLElement* insertionElem = parent->LastChildElement();

			//Even though tracks **should** be in crescent order, tinyxml does not provide a handy way of performing an efficient binary search, so we loop over all elements instead
			for (tinyxml2::XMLElement* child = parent->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
			{
				int trackId = RXML::getIntElement(child, "TrackID");

				if (trackId < id)
					insertionElem = child;
				else if (trackId == id)
					return nullptr;
				else
					break;
			}

			if (insertionElem != nullptr)
			{
				tinyxml2::XMLElement* element = parent->GetDocument()->NewElement("TimeActTrack");
				parent->InsertAfterChild(insertionElem, element);

				trackExport->m_xmlElement = element;
			}
			else
				trackExport->m_xmlElement = parent->InsertNewChildElement("TimeActTrack");

			trackExport->m_xmlElement->SetAttribute("numEventGroups", 0);

			RXML::createIntElement(trackExport->m_xmlElement, "TrackID", id);
			RXML::createIntElement(trackExport->m_xmlElement, "numFrames", numFrames);
			RXML::createIntElement(trackExport->m_xmlElement, "fps", fps);

			return trackExport;
		}

		TimeActGroupExportXML* TimeActTrackExportXML::addEventGroup(int groupId)
		{
			int eventGroupCount = this->getNumEventGroups();
			this->m_xmlElement->SetAttribute("numEventGroups", ++eventGroupCount);

			TimeActGroupExportXML* eventGroupExport = TimeActGroupExportXML::create(this, this->m_xmlElement, groupId);
			this->m_eventGroups.push_back(eventGroupExport);

			return eventGroupExport;
		}

		void TimeActTrackExportXML::deleteGroup(int idx)
		{
			this->m_eventGroups[idx]->clear();
			this->m_xmlElement->DeleteChild(this->m_eventGroups[idx]->m_xmlElement);
			this->m_eventGroups.erase(this->m_eventGroups.begin() + idx);

			int numGroups = this->getNumEventGroups();
			this->m_xmlElement->SetAttribute("numEventGroups", --numGroups);
		}

		void TimeActTrackExportXML::clear()
		{
			int numGroups = this->getNumEventGroups();

			while (numGroups > 0)
			{
				this->deleteGroup(numGroups - 1);

				numGroups--;
			}
		}

		int TimeActTrackExportXML::getTrackId()
		{
			return RXML::getIntElement(this->m_xmlElement, "TrackID");
		}

		int TimeActTrackExportXML::getFps()
		{
			return RXML::getIntElement(this->m_xmlElement, "fps");
		}

		int TimeActTrackExportXML::getFrameCount()
		{
			return RXML::getIntElement(this->m_xmlElement, "numFrames");
		}

		int TimeActTrackExportXML::getNumEventGroups()
		{
			return this->m_xmlElement->FindAttribute("numEventGroups")->IntValue();
		}

		TimeActTrackListExportXML* TimeActTrackListExportXML::create(TimeActExportXML* owner, tinyxml2::XMLElement* parent)
		{
			TimeActTrackListExportXML* trackListExport = new TimeActTrackListExportXML;

			trackListExport->m_xmlElement = parent->InsertNewChildElement("TimeActTrackList");

			trackListExport->m_xmlElement->SetAttribute("numTracks", 0);

			return trackListExport;
		}

		int TimeActTrackListExportXML::getNumTracks()
		{
			return this->m_xmlElement->FindAttribute("numTracks")->IntValue();
		}

		TimeActTrackExportXML* TimeActTrackListExportXML::timeActTrackLookup(int id)
		{
			for (size_t i = 0; i < this->getNumTracks(); i++)
			{
				TimeActTrackExportXML* track = this->getTrack(i);

				if (track->getTrackId() == id)
					return track;
			}

			return nullptr;
		}

		TimeActTrackExportXML* TimeActTrackListExportXML::addTrack(int id, int fps, int numFrames)
		{
			int numTracks = this->getNumTracks();
			this->m_xmlElement->SetAttribute("numTracks", ++numTracks);

			TimeActTrackExportXML* trackExport = TimeActTrackExportXML::create(this, this->m_xmlElement, id, fps, numFrames);

			if (trackExport == nullptr)
			{
				g_appLog->alertMessage(MsgLevel_Error, "A TimeActTrack with the specified ID already exists");
				return nullptr;
			}

			this->m_tracks.push_back(trackExport);

			std::sort(this->m_tracks.begin(), this->m_tracks.end(), isTrackLessThan);

			return trackExport;
		}

		void TimeActTrackListExportXML::deleteTrack(int idx)
		{
			this->m_tracks[idx]->clear();
			this->m_xmlElement->DeleteChild(this->getTrack(idx)->m_xmlElement);
			this->m_tracks.erase(this->m_tracks.begin() + idx);

			int numTracks = this->getNumTracks();
			this->m_xmlElement->SetAttribute("numTracks", --numTracks);
		}

		TimeActExportXML* TimeActExportXML::create(std::string filepath, std::string name, int fileId)
		{
			TimeActExportXML* taeExport = new TimeActExportXML;

			taeExport->m_xmlDoc = new tinyxml2::XMLDocument;

			taeExport->m_xmlElement = taeExport->m_xmlDoc->NewElement("TimeAct");
			taeExport->m_dstFileName = filepath;

			taeExport->m_xmlDoc->InsertEndChild(taeExport->m_xmlElement);

			taeExport->m_xmlElement->SetAttribute("name", name.c_str());
			RXML::createIntElement(taeExport->m_xmlElement, "fileID", fileId);

			taeExport->setSibFile(0, "");
			taeExport->setSkeletonFile(0, "");

			return taeExport;
		}

		std::string TimeActExportXML::getName()
		{
			return this->m_xmlElement->FindAttribute("name")->Value();
		}

		int TimeActExportXML::getFileId()
		{
			return RXML::getIntElement(this->m_xmlElement, "fileID");
		}

		int TimeActExportXML::getSibFileId()
		{
			tinyxml2::XMLElement* element = this->m_xmlElement->FirstChildElement("sibFile");
			
			return element->FindAttribute("fileID")->IntValue();
		}

		std::string TimeActExportXML::getSibName()
		{
			tinyxml2::XMLElement* element = this->m_xmlElement->FirstChildElement("sibFile");

			return element->FindAttribute("name")->Value();
		}

		int TimeActExportXML::getSkeletonId()
		{
			tinyxml2::XMLElement* element = this->m_xmlElement->FirstChildElement("skeletonFile");

			return element->FindAttribute("fileID")->IntValue();
		}

		std::string TimeActExportXML::getSkeletonName()
		{
			tinyxml2::XMLElement* element = this->m_xmlElement->FirstChildElement("skeletonFile");

			return element->FindAttribute("name")->Value();
		}

		void TimeActExportXML::setSibFile(int fileId, std::string name)
		{
			tinyxml2::XMLElement* sibFileElement = this->m_xmlElement->FirstChildElement("sibFile");

			if (sibFileElement == nullptr)
				sibFileElement = this->m_xmlElement->InsertNewChildElement("sibFile");

			sibFileElement->SetAttribute("fileID", fileId);
			sibFileElement->SetAttribute("name", name.c_str());
		}

		void TimeActExportXML::setSkeletonFile(int fileId, std::string name)
		{
			tinyxml2::XMLElement* skeletonFileElement = this->m_xmlElement->FirstChildElement("skeletonFile");

			if (skeletonFileElement == nullptr)
				skeletonFileElement = this->m_xmlElement->InsertNewChildElement("skeletonFile");

			skeletonFileElement->SetAttribute("fileID", fileId);
			skeletonFileElement->SetAttribute("name", name.c_str());
		}

		TimeActTrackListExportXML* TimeActExportXML::createTrackList()
		{
			this->m_trackList = TimeActTrackListExportXML::create(this, this->m_xmlElement);

			return this->m_trackList;
		}

		void TimeActExportXML::findEventsWithId(std::vector<TimeActEventExportXML*>& buffer, const int id)
		{
			buffer.clear();

			TimeActTrackListExportXML* trackList = this->m_trackList;

			for (size_t trackIdx = 0; trackIdx < trackList->getNumTracks(); trackIdx++)
			{
				TimeActTrackExportXML* track = trackList->getTrack(trackIdx);

				for (size_t groupIdx = 0; groupIdx < track->getNumEventGroups(); groupIdx++)
				{
					TimeActGroupExportXML* group = track->getEventGroup(groupIdx);

					for (size_t eventIdx = 0; eventIdx < group->getNumEvents(); eventIdx++)
					{
						TimeActEventExportXML* event = group->getEvent(eventIdx);

						if (event->getEventId() == id)
							buffer.push_back(event);
					}
				}
			}
		}

		bool TimeActExportXML::save()
		{
			if (this->m_xmlDoc->SaveFile(this->m_dstFileName.c_str()) == tinyxml2::XML_SUCCESS)
				return true;

			return false;
		}

		void TimeActExportXML::destroy()
		{
			this->m_xmlDoc->Clear();
			this->m_xmlElement = nullptr;

			delete this->m_xmlDoc;
			delete this;
		}
	}
}