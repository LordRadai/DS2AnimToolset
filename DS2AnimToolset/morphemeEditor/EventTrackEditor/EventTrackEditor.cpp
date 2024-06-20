#include "EventTrackEditor.h"
#include "../Application/Application.h"
#include "INI/INI.h"

EventTrackEditor::EventTrack::EventTrack(int numEvents, int eventId, Event* event, char* name, bool is_discrete)
{
    this->m_source = NULL;

    this->m_numEvents = numEvents;
    this->m_eventId = eventId;

    this->m_name = name;

    this->m_discrete = is_discrete;

    this->m_event = new Event[this->m_numEvents];

    for (size_t i = 0; i < numEvents; i++)
    {
        this->m_event[i].m_frameStart = event[i].m_frameStart;
        this->m_event[i].m_duration = event[i].m_duration;
        this->m_event[i].m_value = event[i].m_value;
    }
}

EventTrackEditor::EventTrack::EventTrack(ME::EventTrackExport* src, float len)
{
    this->m_source = src;
    this->m_numEvents = src->getNumEvents();
    this->m_eventId = src->getUserData();
    this->m_name = src->getName();
    this->m_discrete = false;

    if (src->getEventTrackType() == ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE)
        this->m_discrete = true;

    this->m_event = new Event[this->m_numEvents];

    if (!this->m_discrete)
    {
        ME::DurationEventTrackExportXML* discreteSrc = static_cast<ME::DurationEventTrackExportXML*>(src);

        for (size_t i = 0; i < discreteSrc->getNumEvents(); i++)
        {
            this->m_event[i].m_frameStart = RMath::TimeToFrame(discreteSrc->getEvent(i)->getNormalisedStartTime() * len);
            this->m_event[i].m_duration = RMath::TimeToFrame(discreteSrc->getEvent(i)->getNormalisedDuration() * len);
            this->m_event[i].m_value = discreteSrc->getEvent(i)->getUserData();
        }
    }
    else
    {
        ME::DiscreteEventTrackExportXML* discreteSrc = static_cast<ME::DiscreteEventTrackExportXML*>(src);

        for (size_t i = 0; i < discreteSrc->getNumEvents(); i++)
        {
            this->m_event[i].m_frameStart = RMath::TimeToFrame(discreteSrc->getEvent(i)->getNormalisedTime() * len);
            this->m_event[i].m_duration = 0;
            this->m_event[i].m_value = discreteSrc->getEvent(i)->getUserData();
        }
    }
}

bool EventTrackEditor::EventTrack::IsEventActive(int event_idx, int frame)
{
    if (!this->m_discrete)
    {
        if ((frame >= this->m_event[event_idx].m_frameStart) && frame <= (this->m_event[event_idx].m_duration + this->m_event[event_idx].m_frameStart))
            return true;
    }
    else
    {
        if (frame == this->m_event[event_idx].m_frameStart)
            return true;
    }

    return false;
}

void EventTrackEditor::SaveEventTracks()
{
    ME::ExportFactoryXML factory;

    std::wstring dstFilename = RString::ToWide(this->m_animSource->GetTakeList()->getDestFilename());

    ME::TakeListXML* newTakeList = (ME::TakeListXML*)factory.createTakeList(dstFilename.c_str(), dstFilename.c_str());
    ME::TakeExportXML* take = (ME::TakeExportXML*)newTakeList->createTake(RString::ToWide(this->m_animSource->GetTakeList()->getTake(0)->getName()).c_str(), this->m_animSource->GetTakeList()->getTake(0)->getCachedTakeSecondsDuration(), 30, this->m_animSource->GetTakeList()->getTake(0)->getCachedTakeFPS(), this->m_animSource->GetTakeList()->getTake(0)->getClipStart(), this->m_animSource->GetTakeList()->getTake(0)->getClipEnd());

    for (size_t i = 0; i < this->m_eventTracks.size(); i++)
    {
        EventTrack eventTrack = this->m_eventTracks[i];

        if (eventTrack.m_discrete)
        {
            ME::DiscreteEventTrackExportXML* discreteEventTrack = (ME::DiscreteEventTrackExportXML*)take->createEventTrack(eventTrack.m_source->getEventTrackType(), eventTrack.m_source->getGUID(), RString::ToWide(eventTrack.m_source->getName()).c_str(), eventTrack.m_source->getEventTrackChannelID(), eventTrack.m_source->getUserData());

            for (size_t i = 0; i < eventTrack.m_numEvents; i++)
                discreteEventTrack->createEvent(i, RMath::FrameToTime(eventTrack.m_event[i].m_frameStart) / RMath::FrameToTime(this->m_frameMax), eventTrack.m_event[i].m_value);

            delete eventTrack.m_source;
            eventTrack.m_source = discreteEventTrack;
        }
        else
        {
            ME::DurationEventTrackExportXML* durationEventTrack = (ME::DurationEventTrackExportXML*)take->createEventTrack(eventTrack.m_source->getEventTrackType(), eventTrack.m_source->getGUID(), RString::ToWide(eventTrack.m_source->getName()).c_str(), eventTrack.m_source->getEventTrackChannelID(), eventTrack.m_source->getUserData());

            for (size_t i = 0; i < eventTrack.m_numEvents; i++)
                durationEventTrack->createEvent(i, RMath::FrameToTime(eventTrack.m_event[i].m_frameStart) / RMath::FrameToTime(this->m_frameMax), RMath::FrameToTime(eventTrack.m_event[i].m_duration) / RMath::FrameToTime(this->m_frameMax), eventTrack.m_event[i].m_value);

            delete eventTrack.m_source;
            eventTrack.m_source = durationEventTrack;
        }
    }

    this->m_animSource->SetTakeList(newTakeList);

    this->m_reload = true;
}

int EventTrackEditor::GetFrameMin() const
{
    return m_frameMin;
}

int EventTrackEditor::GetFrameMax() const
{
    return m_frameMax;
}

int EventTrackEditor::GetTrackCount() const { return (int)m_eventTracks.size(); }

std::string EventTrackEditor::GetTrackName(int idx) const { return this->m_eventTracks[idx].m_name; }

std::string EventTrackEditor::GetEventLabel(int track_idx, int event_idx) const
{
    return std::to_string(this->m_eventTracks[track_idx].m_event[event_idx].m_value);
}

void EventTrackEditor::AddTrack(int event_id, char* name, bool duration)
{
    GUID guid;
    CoCreateGuid(&guid); 

    if (duration)
        this->m_animSource->GetTakeList()->getTake(0)->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION, RString::GuidToString(guid).c_str(), RString::ToWide(name).c_str(), this->GetTrackCount(), event_id);
    else
        this->m_animSource->GetTakeList()->getTake(0)->createEventTrack(ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE, RString::GuidToString(guid).c_str(), RString::ToWide(name).c_str(), this->GetTrackCount(), event_id);

    g_appLog->DebugMessage(MsgLevel_Debug, "Added EventTrack %s (%s)\n", RString::GuidToString(guid).c_str(), name);

    this->m_reload = true;
};

void EventTrackEditor::DeleteTrack(int idx)
{
    ME::EventTrackExport* toDelete = this->m_animSource->GetTakeList()->getTake(0)->getEventTrack(idx);

    g_appLog->DebugMessage(MsgLevel_Debug, "Deleted EventTrack %s (%s)\n", toDelete->getGUID(), toDelete->getName());

    ME::ExportFactoryXML factory;

    std::wstring dstFilename = RString::ToWide(this->m_animSource->GetTakeList()->getDestFilename());

    ME::TakeListXML* newTakeList = (ME::TakeListXML*)factory.createTakeList(dstFilename.c_str(), dstFilename.c_str());
    ME::TakeExportXML* take = (ME::TakeExportXML*)newTakeList->createTake(RString::ToWide(this->m_animSource->GetTakeList()->getTake(0)->getName()).c_str(), this->m_animSource->GetTakeList()->getTake(0)->getCachedTakeSecondsDuration(), 30, this->m_animSource->GetTakeList()->getTake(0)->getCachedTakeFPS(), this->m_animSource->GetTakeList()->getTake(0)->getClipStart(), this->m_animSource->GetTakeList()->getTake(0)->getClipEnd());

    for (size_t i = 0; i < this->m_animSource->GetTakeList()->getTake(0)->getNumEventTracks(); i++)
    {
        if (i != idx)
        {
            if (!this->m_eventTracks[i].m_discrete)
            {
                ME::DurationEventTrackExportXML* track = (ME::DurationEventTrackExportXML*)this->m_animSource->GetTakeList()->getTake(0)->getEventTrack(i);
                ME::DurationEventTrackExportXML* newTrack = (ME::DurationEventTrackExportXML*)take->createEventTrack(track->getEventTrackType(), track->getGUID(), RString::ToWide(track->getName()).c_str(), track->getEventTrackChannelID(), track->getUserData());

                for (size_t j = 0; j < track->getNumEvents(); j++)
                {
                    ME::DurationEventExport* event = track->getEvent(j);

                    newTrack->createEvent(j, RMath::FrameToTime(this->m_eventTracks[i].m_event[j].m_frameStart) / RMath::FrameToTime(this->m_frameMax), RMath::FrameToTime(this->m_eventTracks[i].m_event[j].m_duration) / RMath::FrameToTime(this->m_frameMax), this->m_eventTracks[i].m_event[j].m_value);
                }
            }
            else
            {
                ME::DiscreteEventTrackExportXML* track = (ME::DiscreteEventTrackExportXML*)this->m_animSource->GetTakeList()->getTake(0)->getEventTrack(i);
                ME::DiscreteEventTrackExportXML* newTrack = (ME::DiscreteEventTrackExportXML*)take->createEventTrack(track->getEventTrackType(), track->getGUID(), RString::ToWide(track->getName()).c_str(), i, track->getUserData());

                for (size_t j = 0; j < track->getNumEvents(); j++)
                {
                    ME::DiscreteEventExport* event = track->getEvent(j);

                    newTrack->createEvent(j, RMath::FrameToTime(this->m_eventTracks[i].m_event[j].m_frameStart) / RMath::FrameToTime(this->m_frameMax), this->m_eventTracks[i].m_event[j].m_value);
                }
            }
        }
    }

    this->m_animSource->SetTakeList(newTakeList);

    this->m_reload = true;
}

void EventTrackEditor::AddEvent(int track_idx, EventTrack::Event event)
{
    EventTrack* track = &this->m_eventTracks[track_idx];

    if (!this->m_eventTracks[track_idx].m_discrete)
    {
        ME::DurationEventTrackExportXML* source = static_cast<ME::DurationEventTrackExportXML*>(track->m_source);

        source->createEvent(source->getNumEvents(), RMath::FrameToTime(event.m_frameStart) / RMath::FrameToTime(this->m_frameMax), RMath::FrameToTime(event.m_duration) / RMath::FrameToTime(this->m_frameMax), event.m_value);

        g_appLog->DebugMessage(MsgLevel_Debug, "Added event to track %s (%.3f, %.3f, %d)\n", this->m_eventTracks[track_idx].m_name.c_str(), RMath::FrameToTime(event.m_frameStart) / RMath::FrameToTime(this->m_frameMax), RMath::FrameToTime(event.m_duration) / RMath::FrameToTime(this->m_frameMax), event.m_value);
    }
    else
    {
        ME::DiscreteEventTrackExportXML* source = static_cast<ME::DiscreteEventTrackExportXML*>(track->m_source);

        source->createEvent(source->getNumEvents(), RMath::FrameToTime(event.m_frameStart) / RMath::FrameToTime(this->m_frameMax), event.m_value);

        g_appLog->DebugMessage(MsgLevel_Debug, "Added event to track %s (%.3f, %d)\n", this->m_eventTracks[track_idx].m_name.c_str(), RMath::FrameToTime(event.m_frameStart) / RMath::FrameToTime(this->m_frameMax), event.m_value);
    }

    this->m_reload = true;
}

void EventTrackEditor::DeleteEvent(int track_idx, int event_idx)
{
    if (!this->m_eventTracks[track_idx].m_discrete)
    {
        ME::DurationEventTrackExportXML* track = (ME::DurationEventTrackExportXML*)this->m_animSource->GetTakeList()->getTake(0)->getEventTrack(track_idx);
        ME::DurationEventExport* toDelete = track->getEvent(event_idx);

        g_appLog->DebugMessage(MsgLevel_Debug, "Delete duration event for track %s (%.3f, %.3f, %d)\n", track->getName(), toDelete->getNormalisedStartTime(), toDelete->getNormalisedDuration(), toDelete->getUserData());       
    }
    else
    {
        ME::DiscreteEventTrackExportXML* track = (ME::DiscreteEventTrackExportXML*)this->m_animSource->GetTakeList()->getTake(0)->getEventTrack(track_idx);
        ME::DiscreteEventExport* toDelete = track->getEvent(event_idx);

        g_appLog->DebugMessage(MsgLevel_Debug, "Delete duration event for track %s (%.3f, %d)\n", track->getName(), toDelete->getNormalisedTime(), toDelete->getUserData());
    }

    ME::ExportFactoryXML factory;

    std::wstring dstFilename = RString::ToWide(this->m_animSource->GetTakeList()->getDestFilename());

    ME::TakeListXML* newTakeList = (ME::TakeListXML*)factory.createTakeList(dstFilename.c_str(), dstFilename.c_str());
    ME::TakeExportXML* take = (ME::TakeExportXML*)newTakeList->createTake(RString::ToWide(this->m_animSource->GetTakeList()->getTake(0)->getName()).c_str(), this->m_animSource->GetTakeList()->getTake(0)->getCachedTakeSecondsDuration(), 30, this->m_animSource->GetTakeList()->getTake(0)->getCachedTakeFPS(), this->m_animSource->GetTakeList()->getTake(0)->getClipStart(), this->m_animSource->GetTakeList()->getTake(0)->getClipEnd());

    for (size_t i = 0; i < this->m_animSource->GetTakeList()->getTake(0)->getNumEventTracks(); i++)
    {
        int idx = 0;

        if (!this->m_eventTracks[i].m_discrete)
        {
            ME::DurationEventTrackExportXML* track = (ME::DurationEventTrackExportXML*)this->m_animSource->GetTakeList()->getTake(0)->getEventTrack(i);
            ME::DurationEventTrackExportXML* newTrack = (ME::DurationEventTrackExportXML*)take->createEventTrack(track->getEventTrackType(), track->getGUID(), RString::ToWide(track->getName()).c_str(), track->getEventTrackChannelID(), track->getUserData());

            for (size_t j = 0; j < track->getNumEvents(); j++)
            {
                ME::DurationEventExport* event = track->getEvent(j);

                if ((i != track_idx) || (j != event_idx))
                {
                    newTrack->createEvent(idx, RMath::FrameToTime(this->m_eventTracks[i].m_event[j].m_frameStart) / RMath::FrameToTime(this->m_frameMax), RMath::FrameToTime(this->m_eventTracks[i].m_event[j].m_duration) / RMath::FrameToTime(this->m_frameMax), this->m_eventTracks[i].m_event[j].m_value);
                    idx++;
                }
            }
        }
        else
        {
            ME::DiscreteEventTrackExportXML* track = (ME::DiscreteEventTrackExportXML*)this->m_animSource->GetTakeList()->getTake(0)->getEventTrack(i);
            ME::DiscreteEventTrackExportXML* newTrack = (ME::DiscreteEventTrackExportXML*)take->createEventTrack(track->getEventTrackType(), track->getGUID(), RString::ToWide(track->getName()).c_str(), i, track->getUserData());

            for (size_t j = 0; j < track->getNumEvents(); j++)
            {
                ME::DiscreteEventExport* event = track->getEvent(j);

                if ((i != track_idx) || (j != event_idx))
                {
                    newTrack->createEvent(idx, RMath::FrameToTime(this->m_eventTracks[i].m_event[j].m_frameStart) / RMath::FrameToTime(this->m_frameMax), this->m_eventTracks[i].m_event[j].m_value);
                    idx++;
                }
            }
        }
    }

    this->m_animSource->SetTakeList(newTakeList);

    this->m_reload = true;
}

void EventTrackEditor::ReloadTracks()
{
    this->m_reload = false;

    this->m_eventTracks.clear();
    this->SetEditedState(false);

    ME::TakeListXML* takeList = this->m_animSource->GetTakeList();
    if (takeList)
    {
        float animLen = this->m_animSource->GetTakeList()->getTake(0)->getCachedTakeSecondsDuration();

        this->m_frameMin = RMath::TimeToFrame(takeList->getTake(0)->getClipStart() * animLen);
        this->m_frameMax = RMath::TimeToFrame(takeList->getTake(0)->getClipEnd() * animLen);

        this->m_lenMult = takeList->getTake(0)->getCachedTakeSecondsDuration() / (takeList->getTake(0)->getClipEnd() - takeList->getTake(0)->getClipStart());
        int track_count = takeList->getTake(0)->getNumEventTracks();

        this->m_eventTracks.reserve(track_count);

        for (size_t i = 0; i < track_count; i++)
        {
            ME::EventTrackExport* eventTrackXML = takeList->getTake(0)->getEventTrack(i);

            this->m_eventTracks.push_back(EventTrackEditor::EventTrack(eventTrackXML, this->m_lenMult));
        }
    }
    else
    {
        g_appLog->DebugMessage(MsgLevel_Debug, "Animation %d has no event tracks associated to it\n", this->m_animSource->GetID());
    }
}

EventTrackEditor::EventTrackEditor() 
{
    this->m_animSource = nullptr;
    INI ini;

    if (!ini.Open(".//Data//res//color//eventrack.ini"))
    {
        g_appLog->DebugMessage(MsgLevel_Error, "Failed to load eventrack.ini\n");

        this->m_colors.m_trackColor = { 0.31f, 0.31f, 0.91f, 1.f };
        this->m_colors.m_trackColorInactive = { 0.22f, 0.22f, 0.44f, 1.f };
        this->m_colors.m_trackColorActive = { 0.39f, 0.39f, 1.f, 1.f };
        this->m_colors.m_trackBoundingBox = { 0.f, 0.f, 0.f, 1.f };
        this->m_colors.m_trackBoundingBoxActive = { 0.f, 0.f, 0.f, 1.f };
        this->m_colors.m_trackTextColor = { 1.f, 1.f, 1.f, 1.f };
        this->m_colors.m_cursorColor = { 0.f, 0.f, 0.f, 0.f };
    }

    this->m_colors.m_trackColor = { (float)ini.GetFloat("Track", "r", 0.31f), (float)ini.GetFloat("Track", "g", 0.31f), (float)ini.GetFloat("Track", "b", 0.91f), (float)ini.GetFloat("Track", "a", 1.f) };
    this->m_colors.m_trackColorInactive = { (float)ini.GetFloat("TrackInactive", "r", 0.22f), (float)ini.GetFloat("TrackInactive", "g", 0.22f), (float)ini.GetFloat("TrackInactive", "b", 0.44f), (float)ini.GetFloat("TrackInactive", "a", 1.f) };
    this->m_colors.m_trackColorActive = { (float)ini.GetFloat("TrackActive", "r", 0.39f), (float)ini.GetFloat("TrackActive", "g", 0.39f), (float)ini.GetFloat("TrackActive", "b", 1.f), (float)ini.GetFloat("TrackActive", "a", 1.f) };
    this->m_colors.m_trackBoundingBox = { (float)ini.GetFloat("TrackBoundingBox", "r", 0.f), (float)ini.GetFloat("TrackBoundingBox", "g", 0.f), (float)ini.GetFloat("TrackBoundingBox", "b", 0.f), (float)ini.GetFloat("TrackBoundingBox", "a", 1.f) };
    this->m_colors.m_trackBoundingBoxActive = { (float)ini.GetFloat("TrackActiveBoundingBox", "r", 0.f), (float)ini.GetFloat("TrackActiveBoundingBox", "g", 0.f), (float)ini.GetFloat("TrackActiveBoundingBox", "b", 0.f), (float)ini.GetFloat("TrackActiveBoundingBox", "a", 1.f) };
    this->m_colors.m_trackTextColor = { (float)ini.GetFloat("TrackText", "r", 1.f), (float)ini.GetFloat("TrackText", "g", 1.f), (float)ini.GetFloat("TrackText", "b", 1.f), (float)ini.GetFloat("TrackText", "a", 1.f) };
    this->m_colors.m_cursorColor = { (float)ini.GetFloat("TrackCursor", "r", 0.f), (float)ini.GetFloat("TrackCursor", "g", 0.f), (float)ini.GetFloat("TrackCursor", "b", 0.f), (float)ini.GetFloat("TrackCursor", "a", 0.f) };

}

void EventTrackEditor::SetEditedState(bool state)
{
    if (this->m_animIdx == -1)
        return;

    if (g_appRootWindow->m_eventTrackEditor->m_edited.size() > this->m_animIdx)
        g_appRootWindow->m_eventTrackEditor->m_edited[this->m_animIdx] = state;
    else
        g_appLog->PanicMessage("Out of bound read while setting edited state (idx=%d, size=%d)\n", this->m_animIdx, g_appRootWindow->m_eventTrackEditor->m_edited.size());
}

void EventTrackEditor::Clear()
{
    this->m_animIdx = -1;
    this->m_frameMax = 0;
    this->m_frameMin = 0;
    this->m_animSource = nullptr;
    this->m_eventTracks.clear();
}

void EventTrackEditor::ResetSelection()
{
    this->m_selectedEvent = -1;
    this->m_selectedTrack = -1;
}